//-----------------------------------------------------------------------------------
//	Developed by TABOT.inc.
//-----------------------------------------------------------------------------------
/**
*	@file	compression.cpp
*	@brief	圧縮プログラム
*	@todo	大きいファイルを読み込むことはできるが圧縮の処理を施すことが出来ていないため
*			その処理層を実装しなくてはならない
*			圧縮の判別につかう max_equal とmax_index を-1で初期化したり、引き算することを
*			条件としているためunsigned int にするとエラーが吐かれてしまう
*			圧縮率を高めるためにハフマン圧縮と組み合わせることも考慮に入れる
*/
//-----------------------------------------------------------------------------------

#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include <vector>
#include <iterator>
#include <stdio.h>

using namespace std;


class LZSS
{
public:
	/**
	*	圧縮のキーとなる最も出現頻度が低い数値を求める
	*	@param	indata	読み込んだファイルのデータ
	*	@return	encode_key	解凍する際に鍵となる数値
	*/
	int Search_Key(vector<unsigned char> &indata);

	/**
	*	保存のためのヘッダーを作成する
	*	圧縮キー、元データの拡張子を保存し、書き込み用の配列に書き込む
	*	@param	readfilepath	読み込んだファイル名
	*	@param	writedata	書き込み用の配列
	*	@param	encode_key	圧縮キー
	*	@return	writedata	ヘッダー保存後の書き込み配列	
	*/
	vector<unsigned char> Make_Header(string &readfilepath, vector <unsigned char> &writedata, int encode_key);

	/**
	*	LZSS圧縮の実装部分
	*	パターンが一致する部分を256の要素範囲で探索して
	*	圧縮キー、一致パターン開始位置、一致要素数 の順で書き込み用配列に保存していく
	*	一致パターンがなかった場合はそのままの状態で書き込む
	*	@param	indata	読み込んだファイルの保存配列
	*	@param	outdata	書き込み用の配列
	*	@param	encode_key	圧縮キー
	*	@return	outdata	圧縮後のデータを保存した配列	
	*/
	vector<unsigned char> Compression(vector<unsigned char> &indata, vector<unsigned char> &outdata, const int encode_key);

protected:
};


class FileEditor
{    
public:

    /**
     *  引数で渡されたファイル名のファイルを開く
     *  ひらけなかった場合はエラーメッセージを表示し終了する exit(1)
     *  その後動的にメモリを確保する
     *  @param  path    ファイル名のパス
     *  @param  data    メモリ
     *  @return fl  読み込まれたファイルデータ
     */
    vector<unsigned char> File_Open(string &readfilepath, vector<unsigned char> &indata);
    
    /** 
     *  読み込んだファイルのサイズを調べて繰り上がった分以外を戻り値として返す関数
     *  ファイルの終端インデクスから繰り上がりサイズを引くことでファイルサイズを求める
	 *	ファイルサイズがint 以上になった場合、桁上がりをup_file_sizeに保存
     *  @param fl 読み込んだファイルポインタ
	 *	@param up_number 繰り上がり回数
     *  @return size ファイルサイズから繰り上がり分を抜いたサイズ
     */
    int File_Size(ifstream &fl, int up_number);

	/** 
     *  読み込んだファイルのサイズを調べてint以上のファイルサイズの場合繰り上がり回数を返す関数
	 *	ファイルサイズがint 以上になった場合
	 *	ファイルの終端インデクスをINT_SIZEで割ることで、桁上がりをカウントする
     *  @param fl 読み込んだファイルポインタ
     *  @return up_number ファイルの繰り上がり回数
     */
	int Up_File_Size(ifstream &fl);
    
	/**
	 *	ファイル名の拡張子を.lzsに書き換え、圧縮後データを書き込む
	 *	@param	writefilepath	保存ファイル名
	 *	@param	outdata	圧縮したデータを保存する配列
	 */
    void File_Save(string &writefilepath, vector<unsigned char> &outdata);

protected:
    unsigned int file_end;		//ファイルの終端インデクス
    unsigned int file_begin;	//ファイルの開始インデクス
    unsigned int file_size;		//繰り上がり後の残り
	unsigned int up_file_size;	//繰り上がり数
};

//////////////////////////////////本文//////////////////////////////////////////////
//LZSSクラス

int LZSS::Search_Key(vector<unsigned char> &indata)
{
	int search[256];
	int encode_key=0;
	int min = 0;

	for (int searchpoint=0; searchpoint<256; searchpoint++)
	{
		search[searchpoint] = 0;
	}
	for (int insize = 0; insize < indata.size(); insize++)
	{
		search[ indata[insize] ]++;
	}

	min = search[0];//初期化
	for(int searchpoint=0; searchpoint<256; searchpoint++)
	{
		if (min > search[searchpoint])
		{
			min = search[searchpoint];
			encode_key = searchpoint;
		}
	}
	//min = min_element(search, search+256);
	//encode_key = min - search;
    return encode_key;
}

vector<unsigned char> LZSS::Make_Header(string &readfilepath, vector<unsigned char> &outdata, const int encode_key)
{
	string::iterator header = find(readfilepath.begin(), readfilepath.end(), '.');
	outdata.push_back((unsigned char)encode_key);
	copy(header, readfilepath.end(), back_inserter(outdata));
	outdata.push_back('\t');

	return outdata;
}

vector<unsigned char> LZSS::Compression(vector<unsigned char> &indata, vector<unsigned char> &outdata, const int encode_key)
{
	int max_equal = -1;
	int max_index = -1;
	int equal_count=0;
	int byte_count=0;
	int data_it=0;
	int data_up=0;

	//繰り上がりを考慮
	#define DATA_NOW (data_it+data_up*INT_MAX)

	while(DATA_NOW < indata.size())
	{
		max_equal=-1;
		max_index=-1;
		for (byte_count = 0; byte_count < 256; byte_count++)
		{
			if (DATA_NOW - byte_count < 0)
			{
			    break;
			}
        
			for (equal_count = 0; equal_count < byte_count; equal_count++)
			{
				//配列以上は読まない
				if (equal_count + DATA_NOW >= indata.size()) 
				{
					break;
				}
				//一致していない場合抜ける
				if (indata[equal_count + DATA_NOW] != indata[equal_count + DATA_NOW - byte_count])   
				{
					break;
				}

			}
			//圧縮になる4バイト以上の一致がある場合のみ圧縮
			if (equal_count >= 4 && max_equal < equal_count)
			{
				max_equal = equal_count;
				max_index = byte_count;
			}
		}

		if (max_index == -1)
		{
			outdata.push_back(indata[DATA_NOW]);
	
			if(indata[DATA_NOW] == encode_key)
			{
				outdata.push_back(indata[DATA_NOW]);
			}
			if(data_it+1 > INT_MAX)
			{
				data_up++;
				data_it = data_it + 1 - INT_MAX;
			}
			else
			{
				data_it++;
			}
		}
	
		else
		{
			outdata.push_back(encode_key);
	
			outdata.push_back(max_index);
	
			//一致数が圧縮キーと一致すると処理が混乱するため
			//+1することにより判別可能にする
			//このため解凍する際は逆に-1を行う
			if (max_index >= encode_key)
			{
				outdata[ outdata.size() -1 ]++;
			}
			outdata.push_back(max_equal);


			if(data_it + max_equal > INT_MAX)
			{
				data_up++;
				data_it = data_it + max_equal - INT_MAX;
			}
			else
			{
				data_it += max_equal;
			}
		}
	}
	
	return outdata;
}




//FileOpenクラス

vector<unsigned char> FileEditor::File_Open(string &readfilepath, vector<unsigned char> &indata)
{
	//cout<<"file_open"<<endl;
	int size=0;
	int upsize=0;

    setlocale(LC_ALL,"japanese");
    ifstream fl(readfilepath.c_str(), ios::binary); 
    if(fl.fail())	//ファイル読み込みエラー処理
	{  
        cout << "Error: cannot open file ("<< readfilepath.c_str()<<")" << endl;
        exit(1);
    }
	upsize = Up_File_Size(fl);
    size = File_Size(fl,upsize);

    indata.resize((upsize * UINT_MAX) + size); //2GB以上のファイルサイズに対応する
    fl.seekg(0,ios::beg).read((char*)&indata[0], indata.size());
	
    return indata;
}

int FileEditor::File_Size(ifstream &fl,const int up_number)
{
	
    fl.seekg(0, ios_base::end);
    file_end = fl.tellg();
    fl.seekg(up_number * UINT_MAX, ios_base::beg);
    file_begin = fl.tellg();
    file_size = (unsigned int)(file_end - file_begin);
	
	cout<<"filesize:"<<file_size<<endl;
    
	return file_size;
}

int FileEditor::Up_File_Size(ifstream &fl)
{
	
	int up_number=0;

	fl.seekg(0, ios_base::end);
    file_end = fl.tellg();
    up_number = (unsigned int)(file_end / UINT_MAX);
    
	return up_number;
}

void FileEditor::File_Save(string &writefilepath, vector<unsigned char> &outdata)
{
	string::iterator header = find(writefilepath.begin(), writefilepath.end(), '.');
	writefilepath.replace(header, writefilepath.end(), ".lzs");
	ofstream fout(writefilepath.c_str(), ios::binary);
	
	fout.write((char*)&outdata[0], outdata.size());
	
	cout << writefilepath.c_str()<< " Compression completed" << endl;
}



//main関数
int main(int argc, const char* argv[]) 
{
    vector<unsigned char> indata;
    vector<unsigned char> outdata;
    string readfilepath;	//入力ファイルパス
	string writefilepath;	//出力ファイルパス
	bool fset = false;
	bool oset = false;

	if (argc > 1){
		for (int i=0; i<argc; i++){
			if (strcmp(argv[i], "-help") == 0){	//ヘルプ機能
				cout << "以下のようにコマンドライン引数を渡してください" << endl;
				cout << "圧縮処理後ファイル名を指定しない場合、圧縮対象ファイル名のまま圧縮処理を行います" << endl;
				cout << "-f" << " 圧縮対象ファイル名" << endl;
				cout << "-o" << " 圧縮後出力ファイル名" << endl;
				exit(0);
			}
			if (strcmp(argv[i], "-f") == 0){	//入力ファイル
				if (argc > i+1){	//不正アクセス処理
					readfilepath = argv[i+1];
					fset = true;
				}
			}
			if (strcmp(argv[i], "-o") == 0){	//出力ファイル
				if (argc > i+1){	//不正アクセス処理
					writefilepath = argv[i+1];
					oset = true;
				}
			}
		}
	}
    if (fset == false){
	    cout << "同じディレクトリにある圧縮したいファイル名を入力してください: "<< endl;
		cin>> readfilepath;
	}
	if (oset == false){
		cout << "圧縮ファイルを入力ファイルと同じにします "<< endl;
		writefilepath = readfilepath;
	}
    
	int encode_key;
	FileEditor fe;
	LZSS lzss;
	indata = fe.File_Open(readfilepath, indata);
	encode_key = lzss.Search_Key(indata);
	outdata = lzss.Make_Header(readfilepath, outdata, encode_key);
	outdata = lzss.Compression(indata, outdata, encode_key);

	string yorn;
	if(outdata.size()>indata.size())
	{
		cout<< "圧縮ファイルのサイズが入力ファイルより" <<outdata.size() - indata.size()<< "バイト大きくなっています" <<endl;
		cout<< "続ける場合は 'y' 処理をやめる場合は 'n' を入力してください" <<endl;
		cout<< "入力:";
		cin >> yorn;
		if(yorn == "n")
		{
			cout<<"圧縮を中断します"<<endl;
			exit(-1);
		}
		else
		{
		}
	}

	fe.File_Save(writefilepath, outdata);

    return 0;
}
