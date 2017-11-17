//---------------------------------------------------------------------------

#ifndef Unit1H
#define Unit1H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
//---------------------------------------------------------------------------
class TForm1 : public TForm
{
__published:	// IDE-managed Components
	TListBox *ListBox1;
	TButton *Button1;
	TLabel *Label1;
	TLabel *Label2;
	TCheckBox *CheckBox1;
	TCheckBox *CheckBox2;
	TCheckBox *CheckBox3;
	TCheckBox *CheckBox4;
	TCheckBox *CheckBox5;
	TCheckBox *CheckBox6;
	TCheckBox *CheckBox7;
	TEdit *Edit1;
	TButton *Button2;
	TLabel *Label3;
	TButton *Button3;
	TLabel *Label4;
	TLabel *Label5;
	TLabel *Label6;
	TLabel *Label7;
	TLabel *Label8;
	TMemo *Memo1;
	TLabel *Label9;
	TButton *Button4;
	TComboBox *ComboBox1;
	TLabel *Label10;
	TLabel *Label11;
	void __fastcall Button1Click(TObject *Sender);
	void __fastcall ListBox1Click(TObject *Sender);
	void __fastcall Button2Click(TObject *Sender);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall Button3Click(TObject *Sender);
	void __fastcall Button4Click(TObject *Sender);
private:	// User declarations
	bool Stop;
	void UpdateTimes(LONG totalTime, LONG remainingTime);
	void UpdateBuffer(LONG usedSystemBuffer, LONG totalSystemBuffer);
	void UpdateProgress(LONG writtenSectors, LONG totalSectors);
protected:
	virtual void __fastcall WndProc(Messages::TMessage &Message);
	void __fastcall OnImapiUpdate(Messages::TMessage &Message);
public:		// User declarations
	__fastcall TForm1(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TForm1 *Form1;
//---------------------------------------------------------------------------
#endif
