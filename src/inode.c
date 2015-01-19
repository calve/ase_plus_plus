#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "drive.h"
#include "mbr.h"
#include "superbloc.h"
#include "inode.h"

unsigned int current_volume;

void read_inode (unsigned int inumber, struct inode_s* inode)
{
	assert(BLOC_SIZE == sizeof(struct inode_s));
	read_bloc(current_volume,inumber, (unsigned char *)inode);
}

void write_inode (unsigned int inumber, struct inode_s *inode)
{
	assert(BLOC_SIZE == sizeof(struct inode_s));
	write_bloc(current_volume, inumber, (unsigned char *)inode);
}

unsigned int create_inode(enum file_type_e type)
{
	struct inode_s inode;
	int i;
	inode.type = type;
	inode.size = 0;
	inode.blocs_double_indirect = 0;
	inode.blocs_indirect = 0;
	for (i=0; i<NB_DIRECTS; i++){
		inode.blocs_directs[i] = 0;
	}
	unsigned int inumber = new_bloc();
        if (!inumber){
          return BLOC_NULL;
        }
	write_inode(inumber, &inode);
	return inumber;
}

int delete_inode(unsigned int inumber)
{
	struct inode_s inode;
	read_inode(inumber, &inode);
	free_blocs(inode.blocs_directs, NB_DIRECTS);
	unsigned int bloc[NB_BLOCS_BY_BLOC]; /* A voir si utile */
	int i;

	if(inode.blocs_indirect)
	{
		read_bloc(current_volume, inumber, (unsigned char *)inode.blocs_indirect);
		free_blocs(inode.blocs_directs, NB_BLOCS_BY_BLOC);
		free_bloc(inode.blocs_indirect);
	}

	if(inode.blocs_double_indirect)
	{
		read_bloc(current_volume, inumber, (unsigned char *)inode.blocs_double_indirect);
		for(i = 0; i < NB_BLOCS_BY_BLOC; i++)
		{
			unsigned int tmp[NB_BLOCS_BY_BLOC];
			read_bloc(current_volume, inode.blocs_double_indirect, (unsigned char *)tmp);
			free_blocs(tmp, NB_BLOCS_BY_BLOC);
			free_bloc(inode.blocs_double_indirect);
		}
		free_blocs(bloc, NB_BLOCS_BY_BLOC);
	}
	free_bloc(inode.blocs_double_indirect);

	free_bloc(inumber);
	return inumber;
}

unsigned initialize_bloc(unsigned adress)
{
	int new_bloc[NB_BLOCS_BY_BLOC];	
	int i;
	
	read_bloc(current_volume, adress, (unsigned char*)new_bloc);
	for (i = 0; i < NB_BLOCS_BY_BLOC; i++){
		new_bloc[i] = 0;
	}
	write_bloc(current_volume, adress, (unsigned char*)new_bloc);
	return adress;
}

unsigned int vbloc_of_fbloc(unsigned int inumber, unsigned int fbloc, bool_t do_allocate)
{
	struct inode_s inode;
	unsigned int block_index = fbloc;
	read_inode(inumber, &inode);

	//direct
	if (block_index < NB_DIRECTS)
	{
		if (inode.blocs_directs[block_index] == 0)
		{
			if(do_allocate)
			{
				inode.blocs_directs[block_index] = initialize_bloc(new_bloc());
				write_inode(inumber,&inode);
				
			}
			else
			{
				return BLOC_NULL;
			}
		}
		return inode.blocs_directs[block_index];
	}
	block_index -= NB_DIRECTS;

	//indirect simple
	if (block_index < NB_BLOCS_BY_BLOC)
	{
		// if the indirect entry in the inode is not allocated yet
		if (inode.blocs_directs == 0)
		{
			if(do_allocate){
				inode.blocs_indirect = initialize_bloc(new_bloc());;
				write_inode(inumber,&inode);
			}
			else 
			{ 
			 	return BLOC_NULL; 
			}
		}
		
		int indirect[NB_BLOCS_BY_BLOC];	
		read_bloc(current_volume, inode.blocs_indirect, (unsigned char *)indirect);
		
		if (indirect[block_index] == 0)
		{
			if(do_allocate){
				indirect[block_index] = initialize_bloc(new_bloc());;
				write_bloc(current_volume, inode.blocs_indirect, (unsigned char*)indirect);
			}
			else 
			{ 
				return BLOC_NULL; 
			}
		}
		return indirect[block_index];
	}
	
	block_index -= NB_BLOCS_BY_BLOC;
	
	//indirect double
	if(block_index < NB_BLOCS_BY_BLOC*NB_BLOCS_BY_BLOC)
	{
		if (inode.blocs_double_indirect == 0)
		{
			if(do_allocate)
			{
				inode.blocs_double_indirect = initialize_bloc(new_bloc());;
				write_inode(inumber,&inode);
			}
			else 
			{
			 	return BLOC_NULL; 
			}
		}	
		int db_indirect_index = block_index / NB_BLOCS_BY_BLOC;
		int indirect_index = block_index % NB_BLOCS_BY_BLOC; 				
		int db_indirect[NB_BLOCS_BY_BLOC];
		read_bloc(current_volume, inode.blocs_double_indirect, (unsigned char*)db_indirect);
		
		if (db_indirect[db_indirect_index] == 0)
		{
			if(do_allocate)
			{
				db_indirect[db_indirect_index] = initialize_bloc(new_bloc());
				write_bloc(current_volume, inode.blocs_double_indirect, (unsigned char*)db_indirect);
			}
			else 
			{ 
			 	return BLOC_NULL; 
			}
		}
		
		int indirect[NB_BLOCS_BY_BLOC];	
		read_bloc(current_volume, db_indirect[db_indirect_index], (unsigned char*)indirect);
		
		if (indirect[indirect_index] == 0)
		{
			if(do_allocate)
			{
				//printf("allocate indirect[%d]\n",indirect_index);
				indirect[indirect_index] = initialize_bloc(new_bloc());;
				write_bloc(current_volume, db_indirect[db_indirect_index], (unsigned char*)indirect);
			}
			else 
			{ 
			 	return BLOC_NULL; 
			}
		}
		return indirect[indirect_index]; 		
	}
	
	fprintf(stderr,"fbloc is too big.\n\tfbloc provided: %d\n\tfbloc max size: %d",fbloc, NB_DIRECTS+NB_BLOCS_BY_BLOC+NB_BLOCS_BY_BLOC*NB_BLOCS_BY_BLOC);
	return -1;
}

void free_blocs(unsigned *blocs, unsigned int size)
{
	int i;
	for(i=0; i<size; i++) 
	{
		if(blocs[i])
		{
			free_bloc(blocs[i]);			
		}
	}

}
