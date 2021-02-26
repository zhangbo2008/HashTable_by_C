/*
 * @author: jelathro
 * @date: 11/2/13
 * 
 * Implementation file for HashTable.
 * 
 * After ten collisions (ie. ten items in 
 * a particular hash items list) the hash 
 * table will expand to the maxCollision 
 * plus the growthFactor.
 */

#include <stdlib.h>

#include "HashTable.h"

HashTable * hashtable_initialize(size_t size, size_t mc, size_t gf, hash_function fn, compare_equal efn){
	size_t i;
	HashTable * ht = (HashTable *) malloc( sizeof(HashTable) );
        
        if (ht == NULL) {
            printf("\nERROR: Cannot create hash table. Terminating...");
            exit(EXIT_FAILURE); // well... any other implementation could be added, instead of this.
        }

	ht->hf = fn;
	ht->eq = efn;
	ht->size = size;
	ht->maxCollisions = mc;
	ht->growthFactor = gf;

	ht->table = (Item **) malloc(size * sizeof( Item * ));// table表示一个指针,
	//指向的是 Item * .所以 需要malloc Item * 然后强制转化为Item **
        
        if (ht->table == NULL) {
            printf("\nERROR: Cannot create hash table. Terminating...");
            exit(EXIT_FAILURE); // well... any other implementation could be added, instead of this.
        }

	for(i=0; i<size; i++){
		ht->table[i] = 0;// 初始化每一个Item * 指针都是 0. 表示不存在.
	}

	return(ht);
}

void * hashtable_get(HashTable * ht, void * key){
	size_t hash = ht->hf(key); // 计算haxi
	Item * next = ht->table[ hash % ht->size ]; //然后计算位置. 再数组中找到Item *

	while(next){         //如果存在这个Item *指针, 那么我们这里面用的是链表方式来处理哈希碰撞. 一直next指针找下去就行了.
		if(ht->eq( next->key, key )){// 如果符合这个key , 就读取val
			return( next->value );
		}else{
			next = next->next;  //否则就是哈希碰撞. 继续next下去即可.
		}
	}

	return( (void *)0 );   //如果这里面就是找不到这个值, 返回 0.
}

int hashtable_destroy(HashTable * ht){
    
        // Avoiding runtime errors. Return a successful destroy when the 
        // hashtable has not been initialized using the 'hashtable_initialize'
        // function.
    
        if (ht == NULL)                 
          return 1;
        
        // From here we know the hashtable has been properly initialized so, 
        // we continue computations.
        
	size_t i;
        // 先析构每一个儿子, 再析构老子
	for(i=0; i<ht->size; i++){
		free(ht->table[i]);
	}

	free(ht->table);
	free(ht);

	return(1);
}

int hashtable_resize(HashTable * ht, size_t size){
	HashTable * newht = hashtable_initialize(size, ht->maxCollisions, ht->growthFactor, ht->hf, ht->eq);
	int i;
	Item * next;

	// Re-enter all the items again into the new hashtable 
	// with the new size.
	for(i=0; i<ht->size; i++){
		if(ht->table[i]){
			for(next=ht->table[i]; next; next=next->next){
				hashtable_add(newht, next->key, next->value);
			}
		}

		hashtable_remove(ht, ht->table[i]->key);
	}

	free(ht->table);
//	newht->size = ht->size;------------------这个经典的问题一定要多体会!!!!!!!!
//https://blog.csdn.net/weixin_34179762/article/details/85831656
//	newht->table = ht->table;
    // 指针给旧的复制回去.............但是这么写不对!!!!!!!!!!
    // 这是初学指针常常的错误.        初始进来ht=60地址, 这里面改20.  但是地址也是传值的.
    //扔回去的ht还是60地址.
    //指针只能改指针的朝向. 只能把60的指针的指向改为20的内容才行. 只能用*ht来改朝向
//--------------这个才是正确的掰指针的方法!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    *ht=*newht;


	return(1);
}

int hashtable_add(HashTable * ht, void * key, void * value){
	size_t hash = ht->hf(key);
	Item * next = ht->table[ hash % ht->size ];
	size_t i = 0;

	while(next){
		// Replace data if key is same
		if(ht->eq( next->key, key )){
			next->value = value;
			return(1);
		}

		next = next->next;
		i++;
	}

	next = (Item *)malloc( sizeof(Item) );
        
        // Making sure that memory was properly allocated for 'next'.
        if (next == NULL) {
            printf("\nERROR: Insufficient memory. Terminating....");
            exit(EXIT_FAILURE); // some other implementation could be used here.
        }
        
	next->key = key;
	next->value = value;
	next->next = ht->table[ hash % ht->size ]; // 把链表的头换一下, 这样能加速访问.
	//根据原理说. 越最近插入的点, 查询越多.
	ht->table[ hash % ht->size ] = next;

	if(i >= ht->maxCollisions){
		hashtable_resize(ht, ht->size + ht->growthFactor);
	}

	return(1);
}

int hashtable_remove(HashTable * ht, void * key){
	size_t hash = ht->hf(key);
	Item * next = ht->table[ hash % ht->size ];
	Item * prev = 0;  //初始化一个控指针.

	while(next){
		if(ht->eq( next->key, key )){
		    //如果匹配成功了.
			if(prev){//prev是这个指针之前的指针. //如果有就进行连接.
				prev->next = next->next;
			}else{ //如果没有就直接, next.next打头即可.
				ht->table[ hash % ht->size ] = next->next;
			}

			free(next);

			return(1);
		}

		prev = next;
		next = next->next;
	}

	return(0);
}
