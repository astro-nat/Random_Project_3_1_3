/* ------------------------------------------------------------------------ */
/* Developer: Andrew Kirfman, Dr. Betatti                                   */
/* Project: CSCE-313 Machine Problem 2                                      */
/*                                                                          */
/* File: ./memtest.c                                                        */
/* ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------ */
/* User Defined Includes                                                    */
/* ------------------------------------------------------------------------ */

#include "ackerman.h"
#include "my_allocator.h"

int main(int argc, char ** argv) 
{
	MyAllocator *my_alloc = new MyAllocator();
	
	
	my_alloc->init_allocator(2 << 6, 2 << 15);
	Addr test_pointer = my_alloc->memory_array->at(2 << 15);
	my_alloc->split_block(test_pointer);
	my_alloc->split_block(test_pointer);
	
	my_alloc->print_array();
	
	exit(0);
	
	
	
	Addr test = my_alloc->my_malloc((2 << 15) - sizeof(header));
	
	my_alloc->print_array();
	
	std::cout << std::endl;

	_exit(0);


  // input parameters (basic block size, memory length)

  // init_allocator(basic block size, memory length)

  ackerman_main(my_alloc);

  // release_allocator()
}