//-----------------------------------------------------------------------------------
//	Developed by TABOT.inc.
//-----------------------------------------------------------------------------------
/**
*	@file	compression.cpp
*	@brief	���k�v���O����
*	@todo	�傫���t�@�C����ǂݍ��ނ��Ƃ͂ł��邪���k�̏������{�����Ƃ��o���Ă��Ȃ�����
*			���̏����w���������Ȃ��Ă͂Ȃ�Ȃ�
*			���k�̔��ʂɂ��� max_equal ��max_index ��-1�ŏ�����������A�����Z���邱�Ƃ�
*			�����Ƃ��Ă��邽��unsigned int �ɂ���ƃG���[���f����Ă��܂�
*			���k�������߂邽�߂Ƀn�t�}�����k�Ƒg�ݍ��킹�邱�Ƃ��l���ɓ����
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
	*	���k�̃L�[�ƂȂ�ł��o���p�x���Ⴂ���l�����߂�
	*	@param	indata	�ǂݍ��񂾃t�@�C���̃f�[�^
	*	@return	encode_key	�𓀂���ۂɌ��ƂȂ鐔�l
	*/
	int Search_Key(vector<unsigned char> &indata);

	/**
	*	�ۑ��̂��߂̃w�b�_�[���쐬����
	*	���k�L�[�A���f�[�^�̊g���q��ۑ����A�������ݗp�̔z��ɏ�������
	*	@param	readfilepath	�ǂݍ��񂾃t�@�C����
	*	@param	writedata	�������ݗp�̔z��
	*	@param	encode_key	���k�L�[
	*	@return	writedata	�w�b�_�[�ۑ���̏������ݔz��	
	*/
	vector<unsigned char> Make_Header(string &readfilepath, vector <unsigned char> &writedata, int encode_key);

	/**
	*	LZSS���k�̎�������
	*	�p�^�[������v���镔����256�̗v�f�͈͂ŒT������
	*	���k�L�[�A��v�p�^�[���J�n�ʒu�A��v�v�f�� �̏��ŏ������ݗp�z��ɕۑ����Ă���
	*	��v�p�^�[�����Ȃ������ꍇ�͂��̂܂܂̏�Ԃŏ�������
	*	@param	indata	�ǂݍ��񂾃t�@�C���̕ۑ��z��
	*	@param	outdata	�������ݗp�̔z��
	*	@param	encode_key	���k�L�[
	*	@return	outdata	���k��̃f�[�^��ۑ������z��	
	*/
	vector<unsigned char> Compression(vector<unsigned char> &indata, vector<unsigned char> &outdata, const int encode_key);

protected:
};


class FileEditor
{    
public:

    /**
     *  �����œn���ꂽ�t�@�C�����̃t�@�C�����J��
     *  �Ђ炯�Ȃ������ꍇ�̓G���[���b�Z�[�W��\�����I������ exit(1)
     *  ���̌㓮�I�Ƀ��������m�ۂ���
     *  @param  path    �t�@�C�����̃p�X
     *  @param  data    ������
     *  @return fl  �ǂݍ��܂ꂽ�t�@�C���f�[�^
     */
    vector<unsigned char> File_Open(string &readfilepath, vector<unsigned char> &indata);
    
    /** 
     *  �ǂݍ��񂾃t�@�C���̃T�C�Y�𒲂ׂČJ��オ�������ȊO��߂�l�Ƃ��ĕԂ��֐�
     *  �t�@�C���̏I�[�C���f�N�X����J��オ��T�C�Y���������ƂŃt�@�C���T�C�Y�����߂�
	 *	�t�@�C���T�C�Y��int �ȏ�ɂȂ����ꍇ�A���オ���up_file_size�ɕۑ�
     *  @param fl �ǂݍ��񂾃t�@�C���|�C���^
	 *	@param up_number �J��オ���
     *  @return size �t�@�C���T�C�Y����J��オ�蕪�𔲂����T�C�Y
     */
    int File_Size(ifstream &fl, int up_number);

	/** 
     *  �ǂݍ��񂾃t�@�C���̃T�C�Y�𒲂ׂ�int�ȏ�̃t�@�C���T�C�Y�̏ꍇ�J��オ��񐔂�Ԃ��֐�
	 *	�t�@�C���T�C�Y��int �ȏ�ɂȂ����ꍇ
	 *	�t�@�C���̏I�[�C���f�N�X��INT_SIZE�Ŋ��邱�ƂŁA���オ����J�E���g����
     *  @param fl �ǂݍ��񂾃t�@�C���|�C���^
     *  @return up_number �t�@�C���̌J��オ���
     */
	int Up_File_Size(ifstream &fl);
    
	/**
	 *	�t�@�C�����̊g���q��.lzs�ɏ��������A���k��f�[�^����������
	 *	@param	writefilepath	�ۑ��t�@�C����
	 *	@param	outdata	���k�����f�[�^��ۑ�����z��
	 */
    void File_Save(string &writefilepath, vector<unsigned char> &outdata);

protected:
    unsigned int file_end;		//�t�@�C���̏I�[�C���f�N�X
    unsigned int file_begin;	//�t�@�C���̊J�n�C���f�N�X
    unsigned int file_size;		//�J��オ���̎c��
	unsigned int up_file_size;	//�J��オ�萔
};

//////////////////////////////////�{��//////////////////////////////////////////////
//LZSS�N���X

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

	min = search[0];//������
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

	//�J��オ����l��
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
				//�z��ȏ�͓ǂ܂Ȃ�
				if (equal_count + DATA_NOW >= indata.size()) 
				{
					break;
				}
				//��v���Ă��Ȃ��ꍇ������
				if (indata[equal_count + DATA_NOW] != indata[equal_count + DATA_NOW - byte_count])   
				{
					break;
				}

			}
			//���k�ɂȂ�4�o�C�g�ȏ�̈�v������ꍇ�݈̂��k
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
	
			//��v�������k�L�[�ƈ�v����Ə������������邽��
			//+1���邱�Ƃɂ�蔻�ʉ\�ɂ���
			//���̂��߉𓀂���ۂ͋t��-1���s��
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




//FileOpen�N���X

vector<unsigned char> FileEditor::File_Open(string &readfilepath, vector<unsigned char> &indata)
{
	//cout<<"file_open"<<endl;
	int size=0;
	int upsize=0;

    setlocale(LC_ALL,"japanese");
    ifstream fl(readfilepath.c_str(), ios::binary); 
    if(fl.fail())	//�t�@�C���ǂݍ��݃G���[����
	{  
        cout << "Error: cannot open file ("<< readfilepath.c_str()<<")" << endl;
        exit(1);
    }
	upsize = Up_File_Size(fl);
    size = File_Size(fl,upsize);

    indata.resize((upsize * UINT_MAX) + size); //2GB�ȏ�̃t�@�C���T�C�Y�ɑΉ�����
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



//main�֐�
int main(int argc, const char* argv[]) 
{
    vector<unsigned char> indata;
    vector<unsigned char> outdata;
    string readfilepath;	//���̓t�@�C���p�X
	string writefilepath;	//�o�̓t�@�C���p�X
	bool fset = false;
	bool oset = false;

	if (argc > 1){
		for (int i=0; i<argc; i++){
			if (strcmp(argv[i], "-help") == 0){	//�w���v�@�\
				cout << "�ȉ��̂悤�ɃR�}���h���C��������n���Ă�������" << endl;
				cout << "���k������t�@�C�������w�肵�Ȃ��ꍇ�A���k�Ώۃt�@�C�����̂܂܈��k�������s���܂�" << endl;
				cout << "-f" << " ���k�Ώۃt�@�C����" << endl;
				cout << "-o" << " ���k��o�̓t�@�C����" << endl;
				exit(0);
			}
			if (strcmp(argv[i], "-f") == 0){	//���̓t�@�C��
				if (argc > i+1){	//�s���A�N�Z�X����
					readfilepath = argv[i+1];
					fset = true;
				}
			}
			if (strcmp(argv[i], "-o") == 0){	//�o�̓t�@�C��
				if (argc > i+1){	//�s���A�N�Z�X����
					writefilepath = argv[i+1];
					oset = true;
				}
			}
		}
	}
    if (fset == false){
	    cout << "�����f�B���N�g���ɂ��鈳�k�������t�@�C��������͂��Ă�������: "<< endl;
		cin>> readfilepath;
	}
	if (oset == false){
		cout << "���k�t�@�C������̓t�@�C���Ɠ����ɂ��܂� "<< endl;
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
		cout<< "���k�t�@�C���̃T�C�Y�����̓t�@�C�����" <<outdata.size() - indata.size()<< "�o�C�g�傫���Ȃ��Ă��܂�" <<endl;
		cout<< "������ꍇ�� 'y' ��������߂�ꍇ�� 'n' ����͂��Ă�������" <<endl;
		cout<< "����:";
		cin >> yorn;
		if(yorn == "n")
		{
			cout<<"���k�𒆒f���܂�"<<endl;
			exit(-1);
		}
		else
		{
		}
	}

	fe.File_Save(writefilepath, outdata);

    return 0;
}
