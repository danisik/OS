#include "io.h"
#include "kernel.h"
#include "handles.h"

size_t Read_Line_From_Console(char *buffer, const size_t buffer_size) {
	kiv_hal::TRegisters registers;
	
	size_t pos = 0;
	while (pos < buffer_size) {
		//read char
		registers.rax.h = static_cast<decltype(registers.rax.l)>(kiv_hal::NKeyboard::Read_Char);
		kiv_hal::Call_Interrupt_Handler(kiv_hal::NInterrupt::Keyboard, registers);
		
		if (!registers.flags.non_zero) break;	//nic jsme neprecetli, 
												//pokud je rax.l EOT, pak byl zrejme vstup korektne ukoncen
												//jinak zrejme doslo k chybe zarizeni

		char ch = registers.rax.l;

		//osetrime zname kody
		switch (static_cast<kiv_hal::NControl_Codes>(ch)) {
			case kiv_hal::NControl_Codes::BS: {
					//mazeme znak z bufferu
					if (pos > 0) pos--;

					registers.rax.h = static_cast<decltype(registers.rax.l)>(kiv_hal::NVGA_BIOS::Write_Control_Char);
					registers.rdx.l = ch;
					kiv_hal::Call_Interrupt_Handler(kiv_hal::NInterrupt::VGA_BIOS, registers);
				}
				break;

			case kiv_hal::NControl_Codes::LF:  break;	//jenom pohltime, ale necteme
			case kiv_hal::NControl_Codes::NUL:			//chyba cteni?
			case kiv_hal::NControl_Codes::CR:  return pos;	//docetli jsme az po Enter


			default: buffer[pos] = ch;
					 pos++;	
					 registers.rax.h = static_cast<decltype(registers.rax.l)>(kiv_hal::NVGA_BIOS::Write_String);
					 registers.rdx.r = reinterpret_cast<decltype(registers.rdx.r)>(&ch);
					 registers.rcx.r = 1;
					 kiv_hal::Call_Interrupt_Handler(kiv_hal::NInterrupt::VGA_BIOS, registers);
					 break;
		}
	}

	return pos;

}

void Open_File(kiv_hal::TRegisters &regs) {
	//rdx je pointer na null - terminated ANSI char string udavajici file_name;
	//rcx jsou flags k otevreni souboru - viz NOpen_File konstanty
	//rdi jsou atributy souboru - viz NFile_Attributes
	//OUT : ax je handle nove otevreneho souboru
}

void Write_File(kiv_hal::TRegisters &regs) {
	//Spravne bychom nyni meli pouzit interni struktury kernelu a zadany handle resolvovat na konkretni objekt, ktery pise na konkretni zarizeni/souboru/roury.
	//Ale protoze je tohle jenom kostra, tak to rovnou biosem posleme na konzoli.
	kiv_hal::TRegisters registers;
	registers.rax.h = static_cast<decltype(registers.rax.h)>(kiv_hal::NVGA_BIOS::Write_String);
	registers.rdx.r = regs.rdi.r;
	registers.rcx = regs.rcx;

	//preklad parametru dokoncen, zavolame sluzbu
	kiv_hal::Call_Interrupt_Handler(kiv_hal::NInterrupt::VGA_BIOS, registers);

	regs.flags.carry |= (registers.rax.r == 0 ? 1 : 0);	//jestli jsme nezapsali zadny znak, tak jiste doslo k nejake chybe
	regs.rax = registers.rcx;	//VGA BIOS nevraci pocet zapsanych znaku, tak predpokladame, ze zapsal vsechny



	//IN : dx je handle souboru, rdi je pointer na buffer, rcx je pocet bytu v bufferu k zapsani
	//OUT : rax je pocet zapsanych bytu
}

void Read_File(kiv_hal::TRegisters &regs) {
	//viz uvodni komentar u Write_File
	regs.rax.r = Read_Line_From_Console(reinterpret_cast<char*>(regs.rdi.r), regs.rcx.r);


	//IN : dx je handle souboru, rdi je pointer na buffer, kam zapsat, rcx je velikost bufferu v bytech
	//OUT : rax je pocet prectenych bytu
}

void Seek(kiv_hal::TRegisters &regs) {
	//IN : dx je handle souboru, rdi je nova pozice v souboru
	//cl konstatna je typ pozice - viz NFile_Seek,
		//		Beginning : od zacatku souboru
		//		Current : od aktualni pozice v souboru
		//		End : od konce souboru
	//ch == Set_Position jenom nastavi pozici
	//ch == Set_Size nastav pozici a nastav velikost souboru na tuto pozici 
	//ch == Get_Position => OUT: rax je pozice v souboru od jeho zacatku
}

void Close_Handle(kiv_hal::TRegisters &regs) {
	//IN : dx  je handle libovolneho typu k zavreni
}

void Delete_File(kiv_hal::TRegisters &regs) {
	//IN : rdx je pointer na null - terminated ANSI char string udavajici file_name
}

void Set_Working_Dir(kiv_hal::TRegisters &regs) {
	//IN : rdx je pointer na null - terminated ANSI char string udavajici novy adresar(muze byt relativni cesta)
}

void Get_Working_Dir(kiv_hal::TRegisters &regs) {
	//IN : rdx je pointer na ANSI char buffer, rcx je velikost buffer
	//OUT : rax pocet zapsanych znaku

	//vytvoreni adresare - vytvori se soubor s atributem adresar
	//smazani adresare - smaze se soubor
	//vypis adresare - otevre se adresar jako read - only soubor a cte se jako binarni soubor obsahujici jenom polozky TDir_Entry
}

void Create_Pipe(kiv_hal::TRegisters &regs) {
	//IN : rdx je pointer na pole dvou Thandle - prvni zapis a druhy pro cteni z pipy
}


void Handle_IO(kiv_hal::TRegisters &regs) {

	switch (static_cast<kiv_os::NOS_File_System>(regs.rax.l)) {

		case kiv_os::NOS_File_System::Open_File:
			Open_File(regs);		
			break;

		case kiv_os::NOS_File_System::Write_File:
			Write_File(regs);		
			break;

		case kiv_os::NOS_File_System::Read_File:
			Read_File(regs);
			break;

		case kiv_os::NOS_File_System::Seek:
			Seek(regs);
			break;

		case kiv_os::NOS_File_System::Close_Handle:
			Close_Handle(regs);
			break;

		case kiv_os::NOS_File_System::Delete_File:
			Delete_File(regs);
			break;

		case kiv_os::NOS_File_System::Set_Working_Dir:
			Set_Working_Dir(regs);
			break;

		case kiv_os::NOS_File_System::Get_Working_Dir:
			Get_Working_Dir(regs);
			break;

		case kiv_os::NOS_File_System::Create_Pipe:
			Create_Pipe(regs);
			break;
	}
}



	/* Nasledujici dve vetve jsou ukazka, ze starsiho zadani, ktere ukazuji, jak mate mapovat Windows HANDLE na kiv_os handle a zpet, vcetne jejich alokace a uvolneni

		case kiv_os::scCreate_File: {
			HANDLE result = CreateFileA((char*)regs.rdx.r, GENERIC_READ | GENERIC_WRITE, (DWORD)regs.rcx.r, 0, OPEN_EXISTING, 0, 0);
			//zde je treba podle Rxc doresit shared_read, shared_write, OPEN_EXISING, etc. podle potreby
			regs.flags.carry = result == INVALID_HANDLE_VALUE;
			if (!regs.flags.carry) regs.rax.x = Convert_Native_Handle(result);
			else regs.rax.r = GetLastError();
		}
									break;	//scCreateFile

		case kiv_os::scClose_Handle: {
				HANDLE hnd = Resolve_kiv_os_Handle(regs.rdx.x);
				regs.flags.carry = !CloseHandle(hnd);
				if (!regs.flags.carry) Remove_Handle(regs.rdx.x);				
					else regs.rax.r = GetLastError();
			}

			break;	//CloseFile

	*/