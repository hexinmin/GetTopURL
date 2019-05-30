
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <list>
#include <fstream>
#include <string>
#include <unordered_map>

#define MAX_BUFFER_LEN 4096

using namespace std;
// cal hash
unsigned int BKDRHash(char * str)
{
    unsigned int seed = 131;
    unsigned int hash = 0;
    while(*str)
    {
        hash = hash * seed + *str;
        str++;
    }
    return (hash & 0x7FFFFFFF);
}

struct HeapItem
{
    unsigned long long count;
    std::list<std::string> items;
    HeapItem()
    {
        count = 0;
    }

    void swap(struct HeapItem* hi)
    {
        unsigned long long tmp = this->count;
        this->count = hi->count;
        hi->count = tmp;
        
        this->items.swap(hi->items);
    }
};

class GetTopN
{
public:
    GetTopN(unsigned int bucket_size, unsigned int N)
    {
        m_bucket_size = bucket_size;
        m_current_bucket = 0;
        m_top_n = N;
        m_heap_sort_array = new HeapItem[m_top_n];
        m_all_count_number.insert(std::pair<unsigned long long, unsigned int>(0L, 0));
    }

    ~GetTopN()
    {
        delete [] m_heap_sort_array;
    } 

    bool SwitchToNextBucket()
    {
        return m_current_bucket++ < m_bucket_size;
    } 

    void Input(char* url)
    {
        if(BKDRHash(url)%m_bucket_size == m_current_bucket)
        {
           std::string s(url);
           store(s);
        }
    }

    void update_heap()
    {
        for(unordered_map<std::string,unsigned long long>::iterator it = m_all_items.begin(); 
            it != m_all_items.end();it++)
        {
            //printf("%s %llu\n", it->first.c_str(),it->second);
            std::string s(it->first.c_str());
            if(update_heap_top(s, it->second))
            {
                adjust_heap(m_top_n);
            }
        }
        
        m_all_items.clear();
    }

    void DumpItems()
    {
        for(unordered_map<std::string,unsigned long long>::iterator it = m_all_items.begin(); 
            it != m_all_items.end();it++)
        {
            printf("%s %llu\n", it->first.c_str(),it->second);
        }
    }
 
    void DumpHeap()
    {
        int ranking = 0;
        for(unsigned int i = 0; i < m_top_n; i ++)
        {
            for(std::list<std::string>::iterator it = m_heap_sort_array[i].items.begin(); it != 
                   m_heap_sort_array[i].items.end(); it++)
            {
                if(it == m_heap_sort_array[i].items.begin())
                {
                    ranking ++;
                } 
                printf("ranking %d count: %llu url:%s\n",ranking, m_heap_sort_array[i].count, it->c_str());
            }
        }
    }
    
public:
    void store(std::string& s)
    {
        unordered_map<std::string, unsigned long long>::iterator it =
             m_all_items.find(s);
        if(it == m_all_items.end())
        {
           m_all_items.insert(std::pair<std::string,int>(s,1));
        }    
        else
        {
            it->second ++;
        }
    }    

    bool update_heap_top(std::string& s, unsigned long long c)
    {
        unordered_map<unsigned long long, unsigned int>::iterator it;
        struct HeapItem & top = m_heap_sort_array[0];
        if(top.count > c)
        {
            return false; // no need to adjust heap
        } 
        else if( (it=m_all_count_number.find(c)) != m_all_count_number.end())
        {
            // merge
            struct HeapItem & merge = m_heap_sort_array[it->second];
            merge.items.push_back(s);
            return false;
        } 
        else
        {
            // replace the top element of the heap
            m_all_count_number.erase(m_all_count_number.find(top.count));
            m_all_count_number.insert(std::pair<unsigned long long, unsigned int>(c,0));
            top.items.clear();
            top.count = c;
            top.items.push_back(s);
            return true; // top of heap have been changed, must adjust heap
        }
    }

    void adjust_heap(unsigned int end)
    {
        unsigned int i = 0;
        for(unsigned int k = 2*i + 1; k < end; k = k*2 +1)
        {
            if((k + 1 < end) && m_heap_sort_array[k].count > m_heap_sort_array[k+1].count)
            {
                k++;
            }

            if(m_heap_sort_array[i].count > m_heap_sort_array[k].count)
            {
                m_all_count_number[m_heap_sort_array[i].count] = k;
                m_all_count_number[m_heap_sort_array[k].count] = i;
                m_heap_sort_array[i].swap(m_heap_sort_array + k);
                i = k;
            }
            else
            {
                break;
            }
        }
    }

    void heap_sort()
    {
        unsigned int end = m_top_n;
        while(end > 1)
        {
            m_heap_sort_array[0].swap(m_heap_sort_array + end - 1); 
            adjust_heap(end - 1); 
            end --; 
        } 
    }

    unsigned int m_bucket_size;
    unsigned int m_current_bucket;
    unsigned int m_top_n;
    unordered_map<std::string, unsigned long long> m_all_items;
    unordered_map<unsigned long long, unsigned int> m_all_count_number;
    struct HeapItem * m_heap_sort_array;
};

int main(int argc, char** argv)
{
    int bucket_count = atoi(argv[2]); // the bucket count
    int top_count = atoi(argv[3]); // get 'top N'
    GetTopN gtn(bucket_count,top_count);    
    while(true)
    {
        std::ifstream fin(argv[1], std::ios::in);
        char line[MAX_BUFFER_LEN];
        while(fin.getline(line,sizeof(line)))
        {
            gtn.Input(line);
        }

        if(!gtn.SwitchToNextBucket())
        {
            break;
        }
       
        gtn.update_heap();
    } 
    //gtn.DumpHeap();
    //printf("-------------------------\n");
    gtn.heap_sort();
    gtn.DumpHeap();
    
    return 0;
}

