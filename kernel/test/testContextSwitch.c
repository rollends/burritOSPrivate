
void test_task();
void asm_contextSwitch( void (**task)(), unsigned int* psr, unsigned int** stack );

int main( )
{
	unsigned int stack[64];
	unsigned int* userSP = stack + 12;
	void (*task)() = &test_task;
	unsigned int psr = 0; // Pick legitimate values...
	unsigned int i = 0;

	for( i = 0; i < 100; ++i)
		asm_contextSwitch( &task, &psr, &userSP ); // The context switch will try to pop values into registers and will set the SP appropriately!
}


void test_task()
{
	volatile unsigned char c = 255;
	while( --c )
	{
		asm( "swi 0" : : );
	}
}
