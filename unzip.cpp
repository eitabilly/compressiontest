//-----------------------------------------------------------------------------------
//	Developed by TABOT.inc.
//-----------------------------------------------------------------------------------
/**
*	@file	unzip.cpp
*	@brief	�𓀃v���O����
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
	*	�K��Return_Extention�́@��@�ɌĂяo������
	*	���k�f�[�^�z�񂩂爳�k�L�[�A�g���q�A�X�y�[�X�����ׂč폜���Ė߂�l�Ƃ��ĕԂ�
	*	���̍ۈ��k�L�[��ۑ�����
	*	@param	indata	�ǂݍ��񂾔z��
	*	@return	indata	�s�K�v�ȏ����폜�����z��
	*/
	vector<unsigned char> Erase_Extention(vector<unsigned char> &indata);

	/**
	*	�g���q��ۑ����A�߂�l�Ƃ��Ė߂�
	*	@param	indata	�ǂݍ��񂾔z��
	*	@return	extention	�g���q
	*/
	string Return_Extention(vector<unsigned char> &indata);

	/**
	*	�𓀏����̎�������
	*	���k�L�[��z�񂩂�T���A��v�����ꍇ�������n�܂�
	*	������A���������ꍇ�͈��k�L�[���̂��̂ł������Ɣ��f���A���̂܂܏o��
	*	����ȊO�������ꍇ��v�J�n�ʒu�ƈ�v����ǂݎ��A���̏������Ƃɏ������ݔz��ɏ�������
	*	���k�L�[��������Ȃ����肻�̂܂܏o��
	*	@param	indata	�s�K�v�ȏ����폜�����z��
	*	@param	outdata	�������ݗp�z��
	*/
	vector<unsigned char> LZSS_Unzip(vector<unsigned char> &indata, vector<unsigned char> &outdata);

protected:
	string extention;		//���t�@�C���̊g���q
	int data_it;			//�z��̌��ݒn
	int data_up;			//�J��オ�萔
	int encode_key;			//���k�L�[
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
    int File_Size(ifstream &fl,int up_number);

	/** 
     *  �ǂݍ��񂾃t�@�C���̃T�C�Y�𒲂ׂ�int�ȏ�̃t�@�C���T�C�Y�̏ꍇ�J��オ��񐔂�Ԃ��֐�
	 *	�t�@�C���T�C�Y��int �ȏ�ɂȂ����ꍇ
	 *	�t�@�C���̏I�[�C���f�N�X��INT_SIZE�Ŋ��邱�ƂŁA���オ����J�E���g����
     *  @param fl �ǂݍ��񂾃t�@�C���|�C���^
     *  @return up_number �t�@�C���̌J��オ���
     */
	int Up_File_Size(ifstream &fl);
    
	/**
	 *	�t�@�C�����̊g���q�����Ƃɖ߂��A�𓀌�f�[�^����������
	 *	@param	writefilepath	�ۑ��t�@�C����
	 *	@param	extention	�g���q
	 *	@param	outdata	�𓀂����f�[�^��ۑ�����z��
	 */
    void File_Save(string &readfilepath, string &extention, vector<unsigned char> &outdata);

protected:
    unsigned int file_end;		//�t�@�C���̏I�[�C���f�N�X
    unsigned int file_begin;		//�t�@�C���̊J�n�C���f�N�X
    unsigned int file_size;		//�J��オ���̎c��
	unsigned int up_file_size;	//�J��オ�萔
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

	#define DATA_NOW (data_it+data_up*INT_MAX)//�J��オ����l��

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

				//���k�̍ۂ�+1���Ă������l�����ɖ߂�
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
    if(fl.fail())	//�t�@�C���ǂݍ��݃G���[����
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

//main�֐�
int main(int argc, const char* argv[]) 
{
    vector<unsigned char> indata;
    vector<unsigned char> outdata;
    string readfilepath;	//���̓t�@�C���p�X
	string writefilepath;	//�o�̓t�@�C���p�X
	bool fset = false;

	if (argc > 1){
		for (int i=0; i<argc; i++){
			if (strcmp(argv[i], "-help") == 0){	//�w���v�@�\
				cout << "�ȉ��̂悤�ɃR�}���h���C��������n���Ă�������" << endl;
				cout << "�𓀏�����t�@�C�������w�肵�Ȃ��ꍇ�A�𓀑Ώۃt�@�C�����̂܂܈��k�������s���܂�" << endl;
				cout << "-f" << " �𓀑Ώۃt�@�C����" << endl;
				exit(0);
			}
			if (strcmp(argv[i], "-f") == 0){	//���̓t�@�C��
				if (argc > i+1){	//�s���A�N�Z�X����
					readfilepath = argv[i+1];
					fset = true;
				}
			}
		}
	}
    if (fset == false){
	    cout << "�����f�B���N�g���ɂ���𓀂������t�@�C��������͂��Ă�������: "<< endl;
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
