object Form1: TForm1
  Left = 0
  Top = 0
  Caption = 'Form1'
  ClientHeight = 594
  ClientWidth = 595
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  OnDestroy = FormDestroy
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 288
    Top = 39
    Width = 297
    Height = 58
    AutoSize = False
    Caption = 'Label1'
    WordWrap = True
  end
  object Label2: TLabel
    Left = 288
    Top = 110
    Width = 297
    Height = 58
    AutoSize = False
    Caption = 'Label1'
    WordWrap = True
  end
  object Label3: TLabel
    Left = 8
    Top = 317
    Width = 31
    Height = 13
    Caption = 'Label3'
  end
  object Label4: TLabel
    Left = 8
    Top = 352
    Width = 31
    Height = 13
    Caption = 'Label4'
  end
  object Label5: TLabel
    Left = 112
    Top = 352
    Width = 31
    Height = 13
    Caption = 'Label5'
  end
  object Label6: TLabel
    Left = 226
    Top = 352
    Width = 31
    Height = 13
    Caption = 'Label6'
  end
  object Label7: TLabel
    Left = 336
    Top = 352
    Width = 31
    Height = 13
    Caption = 'Label7'
  end
  object Label8: TLabel
    Left = 288
    Top = 188
    Width = 31
    Height = 13
    Caption = 'Label7'
  end
  object Label9: TLabel
    Left = 288
    Top = 317
    Width = 31
    Height = 13
    Caption = 'Label7'
  end
  object Label10: TLabel
    Left = 384
    Top = 188
    Width = 31
    Height = 13
    Caption = 'Label7'
  end
  object Label11: TLabel
    Left = 498
    Top = 188
    Width = 31
    Height = 13
    Caption = 'Label7'
  end
  object ListBox1: TListBox
    Left = 8
    Top = 39
    Width = 249
    Height = 90
    ItemHeight = 13
    TabOrder = 1
    OnClick = ListBox1Click
  end
  object Button1: TButton
    Left = 8
    Top = 8
    Width = 75
    Height = 25
    Caption = #21047#26032
    TabOrder = 0
    OnClick = Button1Click
  end
  object CheckBox1: TCheckBox
    Left = 16
    Top = 184
    Width = 67
    Height = 17
    Caption = 'CD-ROM'
    Enabled = False
    TabOrder = 3
  end
  object CheckBox2: TCheckBox
    Left = 103
    Top = 184
    Width = 67
    Height = 17
    Caption = 'DVD'
    Enabled = False
    TabOrder = 4
  end
  object CheckBox3: TCheckBox
    Left = 190
    Top = 184
    Width = 67
    Height = 17
    Caption = 'DVD-DL'
    Enabled = False
    TabOrder = 5
  end
  object CheckBox4: TCheckBox
    Left = 16
    Top = 232
    Width = 67
    Height = 17
    Caption = 'Writerable'
    Enabled = False
    TabOrder = 6
  end
  object CheckBox5: TCheckBox
    Left = 190
    Top = 232
    Width = 67
    Height = 17
    Caption = 'Blank'
    Enabled = False
    TabOrder = 7
  end
  object CheckBox6: TCheckBox
    Left = 288
    Top = 232
    Width = 67
    Height = 17
    Caption = 'Initialized'
    Enabled = False
    TabOrder = 8
  end
  object CheckBox7: TCheckBox
    Left = 462
    Top = 232
    Width = 67
    Height = 17
    Caption = 'Supported'
    Enabled = False
    TabOrder = 9
  end
  object Edit1: TEdit
    Left = 8
    Top = 280
    Width = 577
    Height = 21
    TabOrder = 10
    Text = 'Edit1'
  end
  object Button2: TButton
    Left = 214
    Top = 385
    Width = 75
    Height = 25
    Caption = #21051#24405'ISO'
    TabOrder = 11
    OnClick = Button2Click
  end
  object Button3: TButton
    Left = 512
    Top = 385
    Width = 75
    Height = 25
    Caption = #20572#27490
    TabOrder = 13
    OnClick = Button3Click
  end
  object Memo1: TMemo
    Left = 8
    Top = 416
    Width = 577
    Height = 170
    Lines.Strings = (
      'Memo1')
    TabOrder = 14
  end
  object Button4: TButton
    Left = 326
    Top = 385
    Width = 75
    Height = 25
    Caption = #21051#24405'Folder'
    TabOrder = 12
    OnClick = Button4Click
  end
  object ComboBox1: TComboBox
    Left = 8
    Top = 147
    Width = 145
    Height = 21
    Style = csDropDownList
    TabOrder = 2
  end
end
