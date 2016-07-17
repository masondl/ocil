#include <cstdio>
#include <cstdlib>

// If succesfull, caller must free data
static bool init_data(int*& data, char* datafile_path, int datasize)
{
	data = (int*) malloc(sizeof(int) * datasize);
	if (data == NULL)
	{
		puts("ERROR, FAILED TO ALLOCATE MEMORY");
		return false;
	}
	
	FILE* datafile = fopen(datafile_path, "r");
	if (datafile == NULL)
	{
		free(data);
		puts("ERROR, FAILED TO OPEN DATA FILE");
		return false;
	}
	
	for (int i = 0; i < datasize; ++i)
	{
		int value;
		if (1 == fscanf(datafile, "%d\n", &value))
		{
			data[i] = value;
		}
		else
		{
			data[i] = 0;
		}
	}
	
	fclose(datafile);
	return true;
}

// If succesfull, caller must free code
static bool init_code(char*& code, char* codefile_path, int& codesize)
{
	FILE* codefile = fopen(codefile_path, "r");
	if (codefile == NULL)
	{
		puts("ERROR, FAILED TO OPEN CODE FILE");
		return false;
	}
	
	fseek(codefile, 0, SEEK_END);
	codesize = ftell(codefile);
	if (codesize == 0)
	{
		puts("ERROR, CODE FILE CAN'T BE EMPTY");
		return false;
	}
	fseek(codefile, 0, SEEK_SET);
	
	code = (char*) malloc(sizeof(char) * codesize);
	if (code == NULL)
	{
		puts("ERROR, FAILED TO ALLOCATE MEMORY");
		fclose(codefile);
		return false;
	}
	
	fread(code, codesize, sizeof(char), codefile);
	
	fclose(codefile);
	return true;
}

static bool handle_command(int*  data, int& dataptr, int datasize,
                           char* code, int& codeptr, int codesize)
{
	static bool test_result = false;
	int ret = 0;
	switch(code[codeptr])
	{
		case '>': // Data Ptr Increment
			if (dataptr < (datasize-1)) ++dataptr;
			break;
		case '<': // Data Ptr Decrement
			if (dataptr > 0) --dataptr;
			break;
		case '+': // Increment
			data[dataptr]++;
			break;
		case '-': // Decrement
			data[dataptr]--;
			break;
		case 'd': // Display (base 10 integer)
			printf("%d\n", data[dataptr]);
			break;
		case 's': // Store (reads from console)
			printf("? ");
			ret = scanf("%d", &(data[dataptr]));
			if (ret != 1)
			{
				puts("ERROR, EXPECTED INTEGER");
				return false;
			}
			break;
		case 't': // Test if zero
			test_result = (data[dataptr] == 0);
			break;
		case 'n': // Test if not zero
			test_result = (data[dataptr] != 0);
			break;
		case '[': // Jump if test forward to matching ']'
			if (test_result)
			{
				int numToMatch = 1;
				while (numToMatch > 0)
				{
					++codeptr;
					if (codeptr > (codesize-1))
					{
						puts("ERROR, '[' with no matching ']' or ')'");
						return false;
					}
					if (code[codeptr] == '[' || 
						code[codeptr] == '(') ++numToMatch;
					if (code[codeptr] == ']' || 
						code[codeptr] == ')') --numToMatch;
				}
			}
			break;
		case ']': // Jump if test backward to matching '['
			if (test_result)
			{
				int numToMatch = 1;
				while (numToMatch > 0)
				{
					--codeptr;
					if (codeptr < 0)
					{
						puts("ERROR, ']' with no matching '[' or '('");
						return false;
					}
					if (code[codeptr] == '[' || 
						code[codeptr] == '(') --numToMatch;
					if (code[codeptr] == ']' || 
						code[codeptr] == ')') ++numToMatch;
				}
			}
			break;
		case '(':
		case ')':
			break;
		case '\n':
			return false;
		default:
			puts("ERROR, UNRECOGIZED CHARACTER");
			return false;
	}
	
	++codeptr;
	return true;
}

int main(int argc, char* argv[])
{
	int   dataptr  = 0;
	int   datasize = 0;
	int*  data     = NULL;
	char* datafile = NULL;
	
	int   codeptr  = 0;
	int   codesize = 0;
	char* code     = NULL;
	char* codefile = NULL;

	// ASSUMES ARGUMENTS ARE VALID
	if (argc == 4)
	{
		datasize = atoi(argv[1]);
		datafile = argv[2];
		codefile = argv[3];
	}
	else
	{
		puts("ERROR, WRONG NUMBER OF ARGUMENTS!");
		return EXIT_FAILURE;
	}

	// Initialize data
	if (!init_data(data, datafile, datasize))
	{
		return EXIT_FAILURE;
	}

	// Initialize code
	if (!init_code(code, codefile, codesize))
	{
		return EXIT_FAILURE;
	}
	
	while (handle_command(data, dataptr, datasize, 
	                      code, codeptr, codesize)) { }

	free(data);
	free(code);
	return EXIT_SUCCESS;
}
