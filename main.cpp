#include <fstream>
#include <iostream>
#include <unordered_map>
#include <boost/filesystem.hpp>
#include <string>
#include <set>
using namespace std;

namespace fs = boost::filesystem;


/*
*       Функции для хэширования, описанны внизу
*/
int binpow (int a, int n);
string hash_fun(char buffer[],long long length);


/*
*       Функция получения строкой абсолютной ссылки
*/
string get_stem(const fs::path &p) { return (absolute(p).string()); }

int main(int argc, char *argv[])
{
    /*
    *       Открытие и проверка того что нам дали каталог
    */
	string pap = (string)(argv[1]);
    if(!fs::is_directory(pap)){
        cout<<"Глупый человек\n";
        return 0;
    }
    /*
    *       Заполнение mp всеми размерами и ссылкками на них
    */
    unordered_map <long long,vector<string>> mp;
 	for (const fs::directory_entry& f : fs::recursive_directory_iterator(pap)) {
  		if (fs::is_regular_file(f.path())) {

    		long long size = (long long)fs::file_size(f.path());
    		string pathi = get_stem(f);
    	    		if (mp.count(size)){
                mp.find(size)->second.push_back(pathi);
            }
            else{
                vector<string> v;
                v.push_back(pathi);
                mp[size] = v;
            };
  		}
  	}

    /*
   	*       Заполнение mp_hash хэшами и ссылкакми на них(пропускаем одиночные файлы)
    */

	unordered_map <string,vector<string>> mp_hash;
    for( auto map_iter = mp.cbegin() ; map_iter != mp.cend() ; ++map_iter ){
        	if(map_iter->second.size()==1)
        		continue;
        	
            for( auto vec_iter = map_iter->second.cbegin() ; vec_iter != map_iter->second.cend() ; ++vec_iter ){
                
                //=========================================
                ifstream fin(*vec_iter);

                int length;

 			 	char * buffer;


  				fin.seekg (0, ios::end);
  				length = fin.tellg();
  				fin.seekg (0, ios::beg);

  				buffer = new char [length];

  				fin.read (buffer,length);
  				fin.close();
  				//=========================================


  				string hash = hash_fun(buffer,length);
				delete[] buffer;


				string link = *vec_iter;
 				if (mp_hash.count(hash)){
                	mp_hash.find(hash)->second.push_back(link);
            	}
            	else{
                	vector<string> v;
                	v.push_back(link);
               		mp_hash[hash] = v;
            	};
            }
    };
       
    /*
	*      Заполнение mp_duble дубликатами и ссылками
    */
    unordered_map <int,vector<string>> mp_duble;
    int groups = 0;
    for( auto map_iter = mp_hash.cbegin() ; map_iter != mp_hash.cend() ; ++map_iter ){
        if(map_iter->second.size()==1)
        	continue;
        else{
            set<int> visited;
            int f_count = 0;
            for( auto vec_iter_first = map_iter->second.cbegin() ; vec_iter_first !=  map_iter->second.cend() ; ++vec_iter_first ){
                f_count++;

                if(visited.count(f_count))
                    continue;
                visited.insert(f_count);
                                
            		ifstream first (*vec_iter_first);
            		int length_first;
            		char * buffer_first;

            		first.seekg (0, ios::end);
            		length_first = first.tellg();

            		first.seekg (0, ios::beg);

            		buffer_first = new char [length_first];

            		first.read(buffer_first,length_first);
					first.close();
                    
                    int f_count_d = 0;
            		for( auto vec_iter_second = vec_iter_first ; vec_iter_second != map_iter->second.cend() ; ++vec_iter_second ){
            			f_count_d++;

                        if(visited.count(f_count_d))
                            continue;


            			ifstream second (*vec_iter_second);
            			int length_second;
            			char * buffer_second;

            			second.seekg (0, ios::end);
            			length_second = second.tellg();

            			second.seekg (0, ios::beg);

            			buffer_second = new char [length_second];
            			second.read(buffer_second,length_second);
            			second.close();
            			int F = 1;
            			for (long l = 0; l<length_second;l++){
							if(buffer_first[l]!=buffer_second[l]){
								F = 0;
								break;
								}
						}
						if(F){

               				//mp_duble[groups] = map_iter->second;

							if (mp_duble.count(groups)){
								if (find(mp_duble.find(groups)->second.begin(), mp_duble.find(groups)->second.end(), *vec_iter_second) == mp_duble.find(groups)->second.end()){
									mp_duble.find(groups)->second.push_back(*vec_iter_second);
                                    visited.insert(f_count_d);
                                    visited.insert(f_count);
                                }
               								
            				}
            				else{

                				vector<string> v;
                				v.push_back(*vec_iter_first);
                				v.push_back(*vec_iter_second);
                                visited.insert(f_count_d);
                                visited.insert(f_count);
                				mp_duble[groups] = v;
            				};
  
						};
            			delete[] buffer_second;
					}

						delete[] buffer_first;
            			groups++;
            };
        };
    }
    /*
    *       Вывод с предложением что то удолить
    */
    int number = 0;
    for( auto map_iter = mp_duble.cbegin() ; map_iter != mp_duble.cend() ; ++map_iter ){
        //if(map_iter->first==0)
        //    break;
        cout <<map_iter->first<< "  \033[0;31mДубликаты найденные:\033[0;34m  \n";
        int count = 1;

        for( auto vec_iter = map_iter->second.cbegin() ; vec_iter != map_iter->second.cend() ; ++vec_iter ){
            cout <<count<<". "<< *vec_iter << "\n" ;
            count++;
            }
        cout << "\n\033[0;37m" ;
        cout << "Какие удолить?(0 - никакие)\n";
        while(1){
            cin>>number;
            if(!number)
            	break;
            int count = 1;
            for( auto vec_iter = map_iter->second.cbegin() ; vec_iter != map_iter->second.cend() ; ++vec_iter ){
                if(number==count){
                	cout <<"удолен "<<count<<". "<< *vec_iter << "\n" ;
                    fs::remove_all((string)*vec_iter);
                	break;
            }
            count++;
            };
        };
    }; 
}

/*
*       Функция хэширования с возведением в степень и суммированием
*/
string hash_fun(char buffer[],long long length){
	unsigned long long score = 0;
	int mod = 13;
    for(int i = 0 ; i<length;i++)
 		score+=binpow((int)buffer[i],i%mod);
 	return to_string(score);
}
/*
*       Быстрая функция возведения в степень
*/
int binpow (int a, int n) {
	int res = 1;

	while (n)
		if (n & 1) {
			res *= a;
			--n;
		}
		else {
			a *= a;
			n >>= 1;
		}
	return res;
}