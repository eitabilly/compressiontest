//-----------------------------------------------------------------------------------
//	Developed by TABOT.inc.
//-----------------------------------------------------------------------------------
/**
*	@file	unzip.cpp
*	@brief	解凍プログラム
*	@todo	
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

class Unzip
{
public:
	/**
	*	必ずReturn_Extentionの　後　に呼び出すこと
	*	圧縮データ配列から圧縮キー、拡張子、スペースをすべて削除して戻り値として返す
	*	その際圧縮キーを保存する
	*	@param	indata	読み込んだ配列
	*	@return	indata	不必要な情報を削除した配列
	*/
	vector<unsigned char> Erase_Extention(vector<unsigned char> &indata);

	/**
	*	拡張子を保存し、戻り値として戻す
	*	@param	indata	読み込んだ配列
	*	@return	extention	拡張子
	*/
	string Return_Extention(vector<unsigned char> &indata);

	/**
	*	解凍処理の実装部分
	*	圧縮キーを配列から探し、一致した場合処理が始まる
	*	もし二連続だった場合は圧縮キーそのものであったと判断し、そのまま出力
	*	それ以外だった場合一致開始位置と一致数を読み取り、その情報をもとに書き込み配列に書き込む
	*	圧縮キーが見つからない限りそのまま出力
	*	@param	indata	不必要な情報を削除した配列
	*	@param	outdata	書き込み用配列
	*/
	vector<unsigned char> LZSS_Unzip(vector<unsigned char> &indata, vector<unsigned char> &outdata);

protected:
	string extention;		//元ファイルの拡張子
	int data_it;			//配列の現在地
	int data_up;			//繰り上がり数
	int encode_key;			//圧縮キー
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
    int File_Size(ifstream &fl,int up_number);

	/** 
     *  読み込んだファイルのサイズを調べてint以上のファイルサイズの場合繰り上がり回数を返す関数
	 *	ファイルサイズがint 以上になった場合
	 *	ファイルの終端インデクスをINT_SIZEで割ることで、桁上がりをカウントする
     *  @param fl 読み込んだファイルポインタ
     *  @return up_number ファイルの繰り上がり回数
     */
	int Up_File_Size(ifstream &fl);
    
	/**
	 *	ファイル名の拡張子をもとに戻し、解凍後データを書き込む
	 *	@param	writefilepath	保存ファイル名
	 *	@param	extention	拡張子
	 *	@param	outdata	解凍したデータを保存する配列
	 */
    void File_Save(string &readfilepath, string &extention, vector<unsigned char> &outdata);

protected:
    unsigned int file_end;		//ファイルの終端インデクス
    unsigned int file_begin;		//ファイルの開始インデクス
    unsigned int file_size;		//繰り上がり後の残り
	unsigned int up_file_size;	//繰り上がり数
};



vector<unsigned char> Unzip::Erase_Extention(vector<unsigned char> &indata)
{
	data_it=0;

	encode_key = indata[data_it];
	data_it++;

	vector<unsigned char>::iterator onetime_extention = find(indata.begin()+1, indata.end(), '\t');
	copy(indata.begin()+1, onetime_extention, back_inserter(extention));

	indata.erase(indata.begin(), onetime_extention +1);
	return indata;
}

string Unzip::Return_Extention(vector<unsigned char> &indata)
{
	data_it=0;

	encode_key = indata[data_it];
	data_it++;

	vector<unsigned char>::iterator onetime_extention = find(indata.begin()+1, indata.end(), '\t');
	copy(indata.begin()+1, onetime_extention, back_inserter(extention));
	return extention;
}

vector<unsigned char> Unzip::LZSS_Unzip(vector<unsigned char> &indata, vector<unsigned char> &outdata)
{
	
	data_it=0;
	data_up= indata.size()/INT_MAX;
	int test=0;

	#define DATA_NOW (data_it+data_up*INT_MAX)//繰り上がりを考慮

	int search_index;
	int equal_count;

	while(DATA_NOW < indata.size())
	{
		if(indata[DATA_NOW] == encode_key)
		{
			data_it++;
			if(indata[DATA_NOW] == encode_key)
			{
				outdata.push_back(indata[DATA_NOW]);
				data_it++;
			}
			else
			{
				search_index = indata[DATA_NOW];
				data_it++;

				//圧縮の際に+1していた数値を元に戻す
				if(search_index >= encode_key)
				{
					search_index--;
				}
					
				equal_count = indata[DATA_NOW];			
				data_it++;

				int outdataup = (int)(outdata.size() / INT_MAX);
				int outdatait = outdata.size() - outdataup * INT_MAX;
				for (int search=0; search<equal_count; search++)
				{
					outdata.push_back(outdata[outdatait+outdataup*INT_MAX - search_index + search]);
				}
			}
		}
		else 
		{
			outdata.push_back(indata[DATA_NOW]);
			data_it++;
		}
	}
	cout<<"outdata"<<endl;
	return outdata;
}


vector<unsigned char> FileEditor::File_Open(string &readfilepath, vector<unsigned char> &indata)
{
	cout<<"file_open"<<endl;
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

    indata.resize(( upsize * INT_MAX ) + size); 
    fl.seekg( 0,ios::beg ).read((char*)&indata[0], indata.size());
	int id=indata.size();

    return indata;
}

int FileEditor::File_Size(ifstream &fl,const int up_number)
{
	
    fl.seekg(0, ios_base::end);
    file_end = fl.tellg();
    fl.seekg(up_number * UINT_MAX, ios_base::beg);
    file_begin = fl.tellg();
    file_size = (unsigned int)(file_end - file_begin);
    
	return file_size;
}

int FileEditor::Up_File_Size(ifstream &fl)
{
	
	int up_number=0;

	fl.seekg(0, ios_base::end);
    file_end = fl.tellg();
	cout<<file_end<<endl;
    up_number = (unsigned int)(file_end / UINT_MAX);
    
	return up_number;
}

void FileEditor::File_Save(string &writefilepath, string &extention, vector<unsigned char> &outdata)
{
	string::iterator filename = find(writefilepath.begin(), writefilepath.end(), '.');
	ofstream fout(writefilepath.replace(filename, writefilepath.end(), extention), ios::binary);
	fout.write((char*)&outdata[0], outdata.size());
	
	cout << writefilepath.c_str()<< " Unzip completed" << endl;
}

//main関数
int main(int argc, const char* argv[]) 
{
    vector<unsigned char> indata;
    vector<unsigned char> outdata;
    string readfilepath;	//入力ファイルパス
	string writefilepath;	//出力ファイルパス
	bool fset = false;

	if (argc > 1){
		for (int i=0; i<argc; i++){
			if (strcmp(argv[i], "-help") == 0){	//ヘルプ機能
				cout << "以下のようにコマンドライン引数を渡してください" << endl;
				cout << "解凍処理後ファイル名を指定しない場合、解凍対象ファイル名のまま圧縮処理を行います" << endl;
				cout << "-f" << " 解凍対象ファイル名" << endl;
				exit(0);
			}
			if (strcmp(argv[i], "-f") == 0){	//入力ファイル
				if (argc > i+1){	//不正アクセス処理
					readfilepath = argv[i+1];
					fset = true;
				}
			}
		}
	}
    if (fset == false){
	    cout << "同じディレクトリにある解凍したいファイル名を入力してください: "<< endl;
		cin>> readfilepath;
	}
	
	writefilepath = readfilepath;
    
	string	fileextention;

	FileEditor fe;
	Unzip unzip;
	indata = fe.File_Open(readfilepath, indata);
	fileextention = unzip.Return_Extention(indata);
	indata = unzip.Erase_Extention(indata);

	outdata = unzip.LZSS_Unzip(indata, outdata);
	fe.File_Save(writefilepath, fileextention, outdata);

    return 0;
}
