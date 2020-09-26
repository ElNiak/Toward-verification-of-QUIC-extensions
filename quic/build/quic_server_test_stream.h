#define _HAS_ITERATOR_DEBUGGING 0
struct ivy_gen {virtual int choose(int rng,const char *name) = 0;};
#include "z3++.h"

/*++
  Copyright (c) Microsoft Corporation

  This hash template is borrowed from Microsoft Z3
  (https://github.com/Z3Prover/z3).

  Simple implementation of bucket-list hash tables conforming roughly
  to SGI hash_map and hash_set interfaces, though not all members are
  implemented.

  These hash tables have the property that insert preserves iterators
  and references to elements.

  This package lives in namespace hash_space. Specializations of
  class "hash" should be made in this namespace.

  --*/

#pragma once

#ifndef HASH_H
#define HASH_H

#ifdef _WINDOWS
#pragma warning(disable:4267)
#endif

#include <string>
#include <vector>
#include <iterator>
#include <fstream>

namespace hash_space {

    unsigned string_hash(const char * str, unsigned length, unsigned init_value);

    template <typename T> class hash {
    public:
        size_t operator()(const T &s) const {
            return s.__hash();
        }
    };

    template <>
        class hash<int> {
    public:
        size_t operator()(const int &s) const {
            return s;
        }
    };

    template <>
        class hash<long long> {
    public:
        size_t operator()(const long long &s) const {
            return s;
        }
    };

    template <>
        class hash<unsigned> {
    public:
        size_t operator()(const unsigned &s) const {
            return s;
        }
    };

    template <>
        class hash<unsigned long long> {
    public:
        size_t operator()(const unsigned long long &s) const {
            return s;
        }
    };

    template <>
        class hash<bool> {
    public:
        size_t operator()(const bool &s) const {
            return s;
        }
    };

    template <>
        class hash<std::string> {
    public:
        size_t operator()(const std::string &s) const {
            return string_hash(s.c_str(), (unsigned)s.size(), 0);
        }
    };

    template <>
        class hash<std::pair<int,int> > {
    public:
        size_t operator()(const std::pair<int,int> &p) const {
            return p.first + p.second;
        }
    };

    template <typename T>
        class hash<std::vector<T> > {
    public:
        size_t operator()(const std::vector<T> &p) const {
            hash<T> h;
            size_t res = 0;
            for (unsigned i = 0; i < p.size(); i++)
                res += h(p[i]);
            return res;
        }
    };

    template <class T>
        class hash<std::pair<T *, T *> > {
    public:
        size_t operator()(const std::pair<T *,T *> &p) const {
            return (size_t)p.first + (size_t)p.second;
        }
    };

    template <class T>
        class hash<T *> {
    public:
        size_t operator()(T * const &p) const {
            return (size_t)p;
        }
    };

    enum { num_primes = 29 };

    static const unsigned long primes[num_primes] =
        {
            7ul,
            53ul,
            97ul,
            193ul,
            389ul,
            769ul,
            1543ul,
            3079ul,
            6151ul,
            12289ul,
            24593ul,
            49157ul,
            98317ul,
            196613ul,
            393241ul,
            786433ul,
            1572869ul,
            3145739ul,
            6291469ul,
            12582917ul,
            25165843ul,
            50331653ul,
            100663319ul,
            201326611ul,
            402653189ul,
            805306457ul,
            1610612741ul,
            3221225473ul,
            4294967291ul
        };

    inline unsigned long next_prime(unsigned long n) {
        const unsigned long* to = primes + (int)num_primes;
        for(const unsigned long* p = primes; p < to; p++)
            if(*p >= n) return *p;
        return primes[num_primes-1];
    }

    template<class Value, class Key, class HashFun, class GetKey, class KeyEqFun>
        class hashtable
    {
    public:

        typedef Value &reference;
        typedef const Value &const_reference;
    
        struct Entry
        {
            Entry* next;
            Value val;
      
        Entry(const Value &_val) : val(_val) {next = 0;}
        };
    

        struct iterator
        {      
            Entry* ent;
            hashtable* tab;

            typedef std::forward_iterator_tag iterator_category;
            typedef Value value_type;
            typedef std::ptrdiff_t difference_type;
            typedef size_t size_type;
            typedef Value& reference;
            typedef Value* pointer;

        iterator(Entry* _ent, hashtable* _tab) : ent(_ent), tab(_tab) { }

            iterator() { }

            Value &operator*() const { return ent->val; }

            Value *operator->() const { return &(operator*()); }

            iterator &operator++() {
                Entry *old = ent;
                ent = ent->next;
                if (!ent) {
                    size_t bucket = tab->get_bucket(old->val);
                    while (!ent && ++bucket < tab->buckets.size())
                        ent = tab->buckets[bucket];
                }
                return *this;
            }

            iterator operator++(int) {
                iterator tmp = *this;
                operator++();
                return tmp;
            }


            bool operator==(const iterator& it) const { 
                return ent == it.ent;
            }

            bool operator!=(const iterator& it) const {
                return ent != it.ent;
            }
        };

        struct const_iterator
        {      
            const Entry* ent;
            const hashtable* tab;

            typedef std::forward_iterator_tag iterator_category;
            typedef Value value_type;
            typedef std::ptrdiff_t difference_type;
            typedef size_t size_type;
            typedef const Value& reference;
            typedef const Value* pointer;

        const_iterator(const Entry* _ent, const hashtable* _tab) : ent(_ent), tab(_tab) { }

            const_iterator() { }

            const Value &operator*() const { return ent->val; }

            const Value *operator->() const { return &(operator*()); }

            const_iterator &operator++() {
                const Entry *old = ent;
                ent = ent->next;
                if (!ent) {
                    size_t bucket = tab->get_bucket(old->val);
                    while (!ent && ++bucket < tab->buckets.size())
                        ent = tab->buckets[bucket];
                }
                return *this;
            }

            const_iterator operator++(int) {
                const_iterator tmp = *this;
                operator++();
                return tmp;
            }


            bool operator==(const const_iterator& it) const { 
                return ent == it.ent;
            }

            bool operator!=(const const_iterator& it) const {
                return ent != it.ent;
            }
        };

    private:

        typedef std::vector<Entry*> Table;

        Table buckets;
        size_t entries;
        HashFun hash_fun ;
        GetKey get_key;
        KeyEqFun key_eq_fun;
    
    public:

    hashtable(size_t init_size) : buckets(init_size,(Entry *)0) {
            entries = 0;
        }
    
        hashtable(const hashtable& other) {
            dup(other);
        }

        hashtable& operator= (const hashtable& other) {
            if (&other != this)
                dup(other);
            return *this;
        }

        ~hashtable() {
            clear();
        }

        size_t size() const { 
            return entries;
        }

        bool empty() const { 
            return size() == 0;
        }

        void swap(hashtable& other) {
            buckets.swap(other.buckets);
            std::swap(entries, other.entries);
        }
    
        iterator begin() {
            for (size_t i = 0; i < buckets.size(); ++i)
                if (buckets[i])
                    return iterator(buckets[i], this);
            return end();
        }
    
        iterator end() { 
            return iterator(0, this);
        }

        const_iterator begin() const {
            for (size_t i = 0; i < buckets.size(); ++i)
                if (buckets[i])
                    return const_iterator(buckets[i], this);
            return end();
        }
    
        const_iterator end() const { 
            return const_iterator(0, this);
        }
    
        size_t get_bucket(const Value& val, size_t n) const {
            return hash_fun(get_key(val)) % n;
        }
    
        size_t get_key_bucket(const Key& key) const {
            return hash_fun(key) % buckets.size();
        }

        size_t get_bucket(const Value& val) const {
            return get_bucket(val,buckets.size());
        }

        Entry *lookup(const Value& val, bool ins = false)
        {
            resize(entries + 1);

            size_t n = get_bucket(val);
            Entry* from = buckets[n];
      
            for (Entry* ent = from; ent; ent = ent->next)
                if (key_eq_fun(get_key(ent->val), get_key(val)))
                    return ent;
      
            if(!ins) return 0;

            Entry* tmp = new Entry(val);
            tmp->next = from;
            buckets[n] = tmp;
            ++entries;
            return tmp;
        }

        Entry *lookup_key(const Key& key) const
        {
            size_t n = get_key_bucket(key);
            Entry* from = buckets[n];
      
            for (Entry* ent = from; ent; ent = ent->next)
                if (key_eq_fun(get_key(ent->val), key))
                    return ent;
      
            return 0;
        }

        const_iterator find(const Key& key) const {
            return const_iterator(lookup_key(key),this);
        }

        iterator find(const Key& key) {
            return iterator(lookup_key(key),this);
        }

        std::pair<iterator,bool> insert(const Value& val){
            size_t old_entries = entries;
            Entry *ent = lookup(val,true);
            return std::pair<iterator,bool>(iterator(ent,this),entries > old_entries);
        }
    
        iterator insert(const iterator &it, const Value& val){
            Entry *ent = lookup(val,true);
            return iterator(ent,this);
        }

        size_t erase(const Key& key)
        {
            Entry** p = &(buckets[get_key_bucket(key)]);
            size_t count = 0;
            while(*p){
                Entry *q = *p;
                if (key_eq_fun(get_key(q->val), key)) {
                    ++count;
                    *p = q->next;
                    delete q;
                }
                else
                    p = &(q->next);
            }
            entries -= count;
            return count;
        }

        void resize(size_t new_size) {
            const size_t old_n = buckets.size();
            if (new_size <= old_n) return;
            const size_t n = next_prime(new_size);
            if (n <= old_n) return;
            Table tmp(n, (Entry*)(0));
            for (size_t i = 0; i < old_n; ++i) {
                Entry* ent = buckets[i];
                while (ent) {
                    size_t new_bucket = get_bucket(ent->val, n);
                    buckets[i] = ent->next;
                    ent->next = tmp[new_bucket];
                    tmp[new_bucket] = ent;
                    ent = buckets[i];
                }
            }
            buckets.swap(tmp);
        }
    
        void clear()
        {
            for (size_t i = 0; i < buckets.size(); ++i) {
                for (Entry* ent = buckets[i]; ent != 0;) {
                    Entry* next = ent->next;
                    delete ent;
                    ent = next;
                }
                buckets[i] = 0;
            }
            entries = 0;
        }

        void dup(const hashtable& other)
        {
            clear();
            buckets.resize(other.buckets.size());
            for (size_t i = 0; i < other.buckets.size(); ++i) {
                Entry** to = &buckets[i];
                for (Entry* from = other.buckets[i]; from; from = from->next)
                    to = &((*to = new Entry(from->val))->next);
            }
            entries = other.entries;
        }
    };

    template <typename T> 
        class equal {
    public:
        bool operator()(const T& x, const T &y) const {
            return x == y;
        }
    };

    template <typename T>
        class identity {
    public:
        const T &operator()(const T &x) const {
            return x;
        }
    };

    template <typename T, typename U>
        class proj1 {
    public:
        const T &operator()(const std::pair<T,U> &x) const {
            return x.first;
        }
    };

    template <typename Element, class HashFun = hash<Element>, 
        class EqFun = equal<Element> >
        class hash_set
        : public hashtable<Element,Element,HashFun,identity<Element>,EqFun> {

    public:

    typedef Element value_type;

    hash_set()
    : hashtable<Element,Element,HashFun,identity<Element>,EqFun>(7) {}
    };

    template <typename Key, typename Value, class HashFun = hash<Key>, 
        class EqFun = equal<Key> >
        class hash_map
        : public hashtable<std::pair<Key,Value>,Key,HashFun,proj1<Key,Value>,EqFun> {

    public:

    hash_map()
    : hashtable<std::pair<Key,Value>,Key,HashFun,proj1<Key,Value>,EqFun>(7) {}

    Value &operator[](const Key& key) {
	std::pair<Key,Value> kvp(key,Value());
	return 
	hashtable<std::pair<Key,Value>,Key,HashFun,proj1<Key,Value>,EqFun>::
        lookup(kvp,true)->val.second;
    }
    };

    template <typename D,typename R>
        class hash<hash_map<D,R> > {
    public:
        size_t operator()(const hash_map<D,R> &p) const {
            hash<D > h1;
            hash<R > h2;
            size_t res = 0;
            
            for (typename hash_map<D,R>::const_iterator it=p.begin(), en=p.end(); it!=en; ++it)
                res += (h1(it->first)+h2(it->second));
            return res;
        }
    };

    template <typename D,typename R>
    inline bool operator ==(const hash_map<D,R> &s, const hash_map<D,R> &t){
        for (typename hash_map<D,R>::const_iterator it=s.begin(), en=s.end(); it!=en; ++it) {
            typename hash_map<D,R>::const_iterator it2 = t.find(it->first);
            if (it2 == t.end() || !(it->second == it2->second)) return false;
        }
        for (typename hash_map<D,R>::const_iterator it=t.begin(), en=t.end(); it!=en; ++it) {
            typename hash_map<D,R>::const_iterator it2 = s.find(it->first);
            if (it2 == t.end() || !(it->second == it2->second)) return false;
        }
        return true;
    }
}
#endif
typedef std::string __strlit;
extern std::ofstream __ivy_out;
void __ivy_exit(int);

template <typename D, typename R>
struct thunk {
    virtual R operator()(const D &) = 0;
    int ___ivy_choose(int rng,const char *name,int id) {
        return 0;
    }
};
template <typename D, typename R, class HashFun = hash_space::hash<D> >
struct hash_thunk {
    thunk<D,R> *fun;
    hash_space::hash_map<D,R,HashFun> memo;
    hash_thunk() : fun(0) {}
    hash_thunk(thunk<D,R> *fun) : fun(fun) {}
    ~hash_thunk() {
//        if (fun)
//            delete fun;
    }
    R &operator[](const D& arg){
        std::pair<typename hash_space::hash_map<D,R>::iterator,bool> foo = memo.insert(std::pair<D,R>(arg,R()));
        R &res = foo.first->second;
        if (foo.second && fun)
            res = (*fun)(arg);
        return res;
    }
};

    class ivy_binary_ser;
    class ivy_binary_deser;


    extern "C" {
    #ifdef _WIN32
    #include "picotls/wincompat.h"
    #endif
    #include "picotls.h"
    #include "picotls/openssl.h"
    #include "picotls/minicrypto.h"
    }

    #include <openssl/pem.h>

    // TODO: put any forward class definitions here

    class tls_callbacks;
    class picotls_connection;



    #include <list>
    #ifndef _WIN32
    #include <netinet/udp.h>
    #include <semaphore.h>
    #endif

    class udp_listener;   // class of threads that listen for connections
    class udp_callbacks;  // class holding callbacks to ivy

    // A udp_config maps endpoint ids to IP addresses and ports.






    class reader;
    class timer;


    struct IntClass {
        IntClass() : val(0) {}
        IntClass(long long val) : val(val) {}
        long long val;
        size_t __hash() const {return val;}
    };
std::ostream& operator<<(std::ostream&s, const IntClass &v) {return s << v.val;}
bool operator==(const IntClass &x, const IntClass &y) {return x.val == y.val;}
class quic_server_test_stream {
  public:
    typedef quic_server_test_stream ivy_class;

    std::vector<std::string> __argv;
#ifdef _WIN32
    void *mutex;  // forward reference to HANDLE
#else
    pthread_mutex_t mutex;
#endif
    void __lock();
    void __unlock();

#ifdef _WIN32
    std::vector<HANDLE> thread_ids;

#else
    std::vector<pthread_t> thread_ids;

#endif
    void install_reader(reader *);
    void install_thread(reader *);
    void install_timer(timer *);
    virtual ~quic_server_test_stream();
    std::vector<int> ___ivy_stack;
    ivy_gen *___ivy_gen;
    int ___ivy_choose(int rng,const char *name,int id);
    virtual void ivy_assert(bool,const char *){}
    virtual void ivy_assume(bool,const char *){}
    virtual void ivy_check_progress(int,int){}
class cid : public IntClass {
public:
    
    cid(){}
    cid(const IntClass &s) : IntClass(s) {}
    cid(long long v) : IntClass(v) {}
    size_t __hash() const { return hash_space::hash<IntClass>()(*this); }
    #ifdef Z3PP_H_
    static z3::sort z3_sort(z3::context &ctx) {return ctx.bv_sort(2);}
    static hash_space::hash_map<IntClass,int> x_to_bv_hash;
    static hash_space::hash_map<int,IntClass> bv_to_x_hash;
    static int next_bv;
    static std::vector<IntClass> nonces;
    static IntClass random_x();
    static int x_to_bv(const IntClass &s){
        if(x_to_bv_hash.find(s) == x_to_bv_hash.end()){
            for (; next_bv < (1<<2); next_bv++) {
                if(bv_to_x_hash.find(next_bv) == bv_to_x_hash.end()) {
                    x_to_bv_hash[s] = next_bv;
                    bv_to_x_hash[next_bv] = s;
                    return next_bv++;
                } 
            }
            std::cerr << "Ran out of values for type cid" << std::endl;
            __ivy_out << "out_of_values(cid,\"" << s << "\")" << std::endl;
            for (int i = 0; i < (1<<2); i++)
                __ivy_out << "value(\"" << bv_to_x_hash[i] << "\")" << std::endl;
            __ivy_exit(1);
        }
        return x_to_bv_hash[s];
    }
    static IntClass bv_to_x(int bv){
        if(bv_to_x_hash.find(bv) == bv_to_x_hash.end()){
            int num = 0;
            while (true) {
                // std::ostringstream str;
                // str << num;
                // IntClass s = str.str();
                IntClass s = random_x();
                if(x_to_bv_hash.find(s) == x_to_bv_hash.end()){
                   x_to_bv_hash[s] = bv;
                   bv_to_x_hash[bv] = s;
                   return s;
                }
                num++;
            }
        }
        return bv_to_x_hash[bv];
    }
    static void prepare() {}
    static void cleanup() {
        x_to_bv_hash.clear();
        bv_to_x_hash.clear();
        next_bv = 0;
    }
    #endif
};    enum stream_kind{unidir,bidir};
    enum role{role__client,role__server};
    enum quic_packet_type{quic_packet_type__initial,quic_packet_type__zero_rtt,quic_packet_type__handshake,quic_packet_type__one_rtt};
    class stream_data : public std::vector<unsigned>{
        public: size_t __hash() const { return hash_space::hash<std::vector<unsigned> >()(*this);};
    };
class tls__handshake{
public:
    struct wrap {
    
        virtual wrap *dup() = 0;
        virtual bool deref() = 0;
        virtual ~wrap() {}
    };
    
    template <typename T> struct twrap : public wrap {
    
        unsigned refs;
    
        T item;
    
        twrap(const T &item) : refs(1), item(item) {}
    
        virtual wrap *dup() {refs++; return this;}
    
        virtual bool deref() {return (--refs) != 0;}
    
    };
    
    int tag;
    
    wrap *ptr;
    
    tls__handshake(){
    tag=-1;
    ptr=0;
    }
    
    tls__handshake(int tag,wrap *ptr) : tag(tag),ptr(ptr) {}
    
    tls__handshake(const tls__handshake&other){
        tag=other.tag;
        ptr = other.ptr ? other.ptr->dup() : 0;
    };
    
    tls__handshake& operator=(const tls__handshake&other){
        tag=other.tag;
        ptr = other.ptr ? other.ptr->dup() : 0;
        return *this;
    };
    
    ~tls__handshake(){if(ptr){if (!ptr->deref()) delete ptr;}}
    
    static int temp_counter;
    
    static void prepare() {temp_counter = 0;}
    
    static void cleanup() {}
    
    size_t __hash() const {
    
        switch(tag) {
    
            case 0: return 0 + hash_space::hash<quic_server_test_stream::tls__client_hello>()(quic_server_test_stream::tls__handshake::unwrap< quic_server_test_stream::tls__client_hello >((*this)));
    
            case 1: return 1 + hash_space::hash<quic_server_test_stream::tls__server_hello>()(quic_server_test_stream::tls__handshake::unwrap< quic_server_test_stream::tls__server_hello >((*this)));
    
            case 2: return 2 + hash_space::hash<quic_server_test_stream::tls__encrypted_extensions>()(quic_server_test_stream::tls__handshake::unwrap< quic_server_test_stream::tls__encrypted_extensions >((*this)));
    
            case 3: return 3 + hash_space::hash<quic_server_test_stream::tls__unknown_message>()(quic_server_test_stream::tls__handshake::unwrap< quic_server_test_stream::tls__unknown_message >((*this)));
    
        }
    
        return 0;
    
    }
    
    template <typename T> static const T &unwrap(const tls__handshake &x) {
    
        return ((static_cast<const twrap<T> *>(x.ptr))->item);
    
    }
    
    template <typename T> static T &unwrap(tls__handshake &x) {
    
         twrap<T> *p = static_cast<twrap<T> *>(x.ptr);
    
         if (p->refs > 1) {
    
             p = new twrap<T> (p->item);
    
         }
    
         return ((static_cast<twrap<T> *>(p))->item);
    
    }
    
};class tls__extension{
public:
    struct wrap {
    
        virtual wrap *dup() = 0;
        virtual bool deref() = 0;
        virtual ~wrap() {}
    };
    
    template <typename T> struct twrap : public wrap {
    
        unsigned refs;
    
        T item;
    
        twrap(const T &item) : refs(1), item(item) {}
    
        virtual wrap *dup() {refs++; return this;}
    
        virtual bool deref() {return (--refs) != 0;}
    
    };
    
    int tag;
    
    wrap *ptr;
    
    tls__extension(){
    tag=-1;
    ptr=0;
    }
    
    tls__extension(int tag,wrap *ptr) : tag(tag),ptr(ptr) {}
    
    tls__extension(const tls__extension&other){
        tag=other.tag;
        ptr = other.ptr ? other.ptr->dup() : 0;
    };
    
    tls__extension& operator=(const tls__extension&other){
        tag=other.tag;
        ptr = other.ptr ? other.ptr->dup() : 0;
        return *this;
    };
    
    ~tls__extension(){if(ptr){if (!ptr->deref()) delete ptr;}}
    
    static int temp_counter;
    
    static void prepare() {temp_counter = 0;}
    
    static void cleanup() {}
    
    size_t __hash() const {
    
        switch(tag) {
    
            case 0: return 0 + hash_space::hash<quic_server_test_stream::tls__unknown_extension>()(quic_server_test_stream::tls__extension::unwrap< quic_server_test_stream::tls__unknown_extension >((*this)));
    
            case 1: return 1 + hash_space::hash<quic_server_test_stream::quic_transport_parameters>()(quic_server_test_stream::tls__extension::unwrap< quic_server_test_stream::quic_transport_parameters >((*this)));
    
        }
    
        return 0;
    
    }
    
    template <typename T> static const T &unwrap(const tls__extension &x) {
    
        return ((static_cast<const twrap<T> *>(x.ptr))->item);
    
    }
    
    template <typename T> static T &unwrap(tls__extension &x) {
    
         twrap<T> *p = static_cast<twrap<T> *>(x.ptr);
    
         if (p->refs > 1) {
    
             p = new twrap<T> (p->item);
    
         }
    
         return ((static_cast<twrap<T> *>(p))->item);
    
    }
    
};    struct tls__unknown_extension {
    unsigned etype;
    stream_data content;
        size_t __hash() const { return hash_space::hash<unsigned>()(etype)+hash_space::hash<stream_data>()(content);}
    };
    struct tls__random {
    unsigned gmt_unix_time;
    stream_data random_bytes;
        size_t __hash() const { return hash_space::hash<unsigned>()(gmt_unix_time)+hash_space::hash<stream_data>()(random_bytes);}
    };
    class vector__tls__cipher_suite__ : public std::vector<unsigned>{
        public: size_t __hash() const { return hash_space::hash<std::vector<unsigned> >()(*this);};
    };
    class vector__tls__compression_method__ : public std::vector<unsigned>{
        public: size_t __hash() const { return hash_space::hash<std::vector<unsigned> >()(*this);};
    };
    class vector__tls__extension__ : public std::vector<tls__extension>{
        public: size_t __hash() const { return hash_space::hash<std::vector<tls__extension> >()(*this);};
    };
    struct tls__client_hello {
    unsigned client_version;
    tls__random rand_info;
    stream_data session_id;
    vector__tls__cipher_suite__ cipher_suites;
    vector__tls__compression_method__ compression_methods;
    vector__tls__extension__ extensions;
        size_t __hash() const { return hash_space::hash<unsigned>()(client_version)+hash_space::hash<tls__random>()(rand_info)+hash_space::hash<stream_data>()(session_id)+hash_space::hash<vector__tls__cipher_suite__>()(cipher_suites)+hash_space::hash<vector__tls__compression_method__>()(compression_methods)+hash_space::hash<vector__tls__extension__>()(extensions);}
    };
    struct tls__server_hello {
    unsigned server_version;
    tls__random rand_info;
    stream_data session_id;
    unsigned the_cipher_suite;
    unsigned the_compression_method;
    vector__tls__extension__ extensions;
        size_t __hash() const { return hash_space::hash<unsigned>()(server_version)+hash_space::hash<tls__random>()(rand_info)+hash_space::hash<stream_data>()(session_id)+hash_space::hash<unsigned>()(the_cipher_suite)+hash_space::hash<unsigned>()(the_compression_method)+hash_space::hash<vector__tls__extension__>()(extensions);}
    };
    struct tls__encrypted_extensions {
    vector__tls__extension__ extensions;
        size_t __hash() const { return hash_space::hash<vector__tls__extension__>()(extensions);}
    };
    struct tls__unknown_message {
    unsigned mtype;
    stream_data unknown_message_bytes;
        size_t __hash() const { return hash_space::hash<unsigned>()(mtype)+hash_space::hash<stream_data>()(unknown_message_bytes);}
    };
    class vector__tls__handshake__ : public std::vector<tls__handshake>{
        public: size_t __hash() const { return hash_space::hash<std::vector<tls__handshake> >()(*this);};
    };
    enum ip__protocol{ip__udp,ip__tcp};
    struct ip__endpoint {
    ip__protocol protocol;
    unsigned addr;
    unsigned port;
        size_t __hash() const { return hash_space::hash<int>()(protocol)+hash_space::hash<unsigned>()(addr)+hash_space::hash<unsigned>()(port);}
    };
    class tls__handshakes : public std::vector<tls__handshake>{
        public: size_t __hash() const { return hash_space::hash<std::vector<tls__handshake> >()(*this);};
    };
    struct tls__handshake_parser__result {
    unsigned long long pos;
    tls__handshakes value;
        size_t __hash() const { return hash_space::hash<unsigned long long>()(pos)+hash_space::hash<tls__handshakes>()(value);}
    };
class frame{
public:
    struct wrap {
    
        virtual wrap *dup() = 0;
        virtual bool deref() = 0;
        virtual ~wrap() {}
    };
    
    template <typename T> struct twrap : public wrap {
    
        unsigned refs;
    
        T item;
    
        twrap(const T &item) : refs(1), item(item) {}
    
        virtual wrap *dup() {refs++; return this;}
    
        virtual bool deref() {return (--refs) != 0;}
    
    };
    
    int tag;
    
    wrap *ptr;
    
    frame(){
    tag=-1;
    ptr=0;
    }
    
    frame(int tag,wrap *ptr) : tag(tag),ptr(ptr) {}
    
    frame(const frame&other){
        tag=other.tag;
        ptr = other.ptr ? other.ptr->dup() : 0;
    };
    
    frame& operator=(const frame&other){
        tag=other.tag;
        ptr = other.ptr ? other.ptr->dup() : 0;
        return *this;
    };
    
    ~frame(){if(ptr){if (!ptr->deref()) delete ptr;}}
    
    static int temp_counter;
    
    static void prepare() {temp_counter = 0;}
    
    static void cleanup() {}
    
    size_t __hash() const {
    
        switch(tag) {
    
            case 0: return 0 + hash_space::hash<quic_server_test_stream::frame__stream>()(quic_server_test_stream::frame::unwrap< quic_server_test_stream::frame__stream >((*this)));
    
            case 1: return 1 + hash_space::hash<quic_server_test_stream::frame__ack>()(quic_server_test_stream::frame::unwrap< quic_server_test_stream::frame__ack >((*this)));
    
            case 2: return 2 + hash_space::hash<quic_server_test_stream::frame__rst_stream>()(quic_server_test_stream::frame::unwrap< quic_server_test_stream::frame__rst_stream >((*this)));
    
            case 3: return 3 + hash_space::hash<quic_server_test_stream::frame__max_stream_id>()(quic_server_test_stream::frame::unwrap< quic_server_test_stream::frame__max_stream_id >((*this)));
    
            case 4: return 4 + hash_space::hash<quic_server_test_stream::frame__connection_close>()(quic_server_test_stream::frame::unwrap< quic_server_test_stream::frame__connection_close >((*this)));
    
            case 5: return 5 + hash_space::hash<quic_server_test_stream::frame__max_stream_data>()(quic_server_test_stream::frame::unwrap< quic_server_test_stream::frame__max_stream_data >((*this)));
    
            case 6: return 6 + hash_space::hash<quic_server_test_stream::frame__stream_id_blocked>()(quic_server_test_stream::frame::unwrap< quic_server_test_stream::frame__stream_id_blocked >((*this)));
    
            case 7: return 7 + hash_space::hash<quic_server_test_stream::frame__crypto>()(quic_server_test_stream::frame::unwrap< quic_server_test_stream::frame__crypto >((*this)));
    
            case 8: return 8 + hash_space::hash<quic_server_test_stream::frame__ping>()(quic_server_test_stream::frame::unwrap< quic_server_test_stream::frame__ping >((*this)));
    
            case 9: return 9 + hash_space::hash<quic_server_test_stream::frame__application_close>()(quic_server_test_stream::frame::unwrap< quic_server_test_stream::frame__application_close >((*this)));
    
            case 10: return 10 + hash_space::hash<quic_server_test_stream::frame__new_connection_id>()(quic_server_test_stream::frame::unwrap< quic_server_test_stream::frame__new_connection_id >((*this)));
    
            case 11: return 11 + hash_space::hash<quic_server_test_stream::frame__path_challenge>()(quic_server_test_stream::frame::unwrap< quic_server_test_stream::frame__path_challenge >((*this)));
    
            case 12: return 12 + hash_space::hash<quic_server_test_stream::frame__path_response>()(quic_server_test_stream::frame::unwrap< quic_server_test_stream::frame__path_response >((*this)));
    
            case 13: return 13 + hash_space::hash<quic_server_test_stream::frame__new_token>()(quic_server_test_stream::frame::unwrap< quic_server_test_stream::frame__new_token >((*this)));
    
            case 14: return 14 + hash_space::hash<quic_server_test_stream::frame__max_data>()(quic_server_test_stream::frame::unwrap< quic_server_test_stream::frame__max_data >((*this)));
    
            case 15: return 15 + hash_space::hash<quic_server_test_stream::frame__stream_blocked>()(quic_server_test_stream::frame::unwrap< quic_server_test_stream::frame__stream_blocked >((*this)));
    
            case 16: return 16 + hash_space::hash<quic_server_test_stream::frame__stop_sending>()(quic_server_test_stream::frame::unwrap< quic_server_test_stream::frame__stop_sending >((*this)));
    
            case 17: return 17 + hash_space::hash<quic_server_test_stream::frame__blocked>()(quic_server_test_stream::frame::unwrap< quic_server_test_stream::frame__blocked >((*this)));
    
            case 18: return 18 + hash_space::hash<quic_server_test_stream::frame__retire_connection_id>()(quic_server_test_stream::frame::unwrap< quic_server_test_stream::frame__retire_connection_id >((*this)));
    
        }
    
        return 0;
    
    }
    
    template <typename T> static const T &unwrap(const frame &x) {
    
        return ((static_cast<const twrap<T> *>(x.ptr))->item);
    
    }
    
    template <typename T> static T &unwrap(frame &x) {
    
         twrap<T> *p = static_cast<twrap<T> *>(x.ptr);
    
         if (p->refs > 1) {
    
             p = new twrap<T> (p->item);
    
         }
    
         return ((static_cast<twrap<T> *>(p))->item);
    
    }
    
};    struct frame__stream {
    bool off;
    bool len;
    bool fin;
    unsigned id;
    unsigned long long offset;
    unsigned long long length;
    stream_data data;
        size_t __hash() const { return hash_space::hash<bool>()(off)+hash_space::hash<bool>()(len)+hash_space::hash<bool>()(fin)+hash_space::hash<unsigned>()(id)+hash_space::hash<unsigned long long>()(offset)+hash_space::hash<unsigned long long>()(length)+hash_space::hash<stream_data>()(data);}
    };
    struct frame__ack__block {
    unsigned gap;
    unsigned blocks;
        size_t __hash() const { return hash_space::hash<unsigned>()(gap)+hash_space::hash<unsigned>()(blocks);}
    };
    class frame__ack__block__arr : public std::vector<frame__ack__block>{
        public: size_t __hash() const { return hash_space::hash<std::vector<frame__ack__block> >()(*this);};
    };
    struct frame__ack {
    unsigned largest_acked;
    int ack_delay;
    frame__ack__block__arr ack_blocks;
        size_t __hash() const { return hash_space::hash<unsigned>()(largest_acked)+hash_space::hash<int>()(ack_delay)+hash_space::hash<frame__ack__block__arr>()(ack_blocks);}
    };
    struct frame__rst_stream {
    unsigned id;
    unsigned err_code;
    unsigned long long final_offset;
        size_t __hash() const { return hash_space::hash<unsigned>()(id)+hash_space::hash<unsigned>()(err_code)+hash_space::hash<unsigned long long>()(final_offset);}
    };
    struct frame__max_stream_id {
    unsigned id;
        size_t __hash() const { return hash_space::hash<unsigned>()(id);}
    };
    struct frame__connection_close {
    unsigned err_code;
    unsigned frame_type;
    unsigned long long reason_phrase_length;
    stream_data reason_phrase;
        size_t __hash() const { return hash_space::hash<unsigned>()(err_code)+hash_space::hash<unsigned>()(frame_type)+hash_space::hash<unsigned long long>()(reason_phrase_length)+hash_space::hash<stream_data>()(reason_phrase);}
    };
    struct frame__max_stream_data {
    unsigned id;
    unsigned long long pos;
        size_t __hash() const { return hash_space::hash<unsigned>()(id)+hash_space::hash<unsigned long long>()(pos);}
    };
    struct frame__stream_id_blocked {
    unsigned id;
        size_t __hash() const { return hash_space::hash<unsigned>()(id);}
    };
    struct frame__crypto {
    unsigned long long offset;
    unsigned long long length;
    stream_data data;
        size_t __hash() const { return hash_space::hash<unsigned long long>()(offset)+hash_space::hash<unsigned long long>()(length)+hash_space::hash<stream_data>()(data);}
    };
    struct frame__ping {
        size_t __hash() const { return 0;}
    };
    struct frame__application_close {
    unsigned err_code;
    unsigned long long reason_phrase_length;
    stream_data reason_phrase;
        size_t __hash() const { return hash_space::hash<unsigned>()(err_code)+hash_space::hash<unsigned long long>()(reason_phrase_length)+hash_space::hash<stream_data>()(reason_phrase);}
    };
    struct frame__new_connection_id {
    unsigned seq_num;
    unsigned retire_prior_to;
    unsigned length;
    cid scid;
    unsigned token;
        size_t __hash() const { return hash_space::hash<unsigned>()(seq_num)+hash_space::hash<unsigned>()(retire_prior_to)+hash_space::hash<unsigned>()(length)+hash_space::hash<cid>()(scid)+hash_space::hash<unsigned>()(token);}
    };
    struct frame__path_challenge {
    stream_data data;
        size_t __hash() const { return hash_space::hash<stream_data>()(data);}
    };
    struct frame__path_response {
    stream_data data;
        size_t __hash() const { return hash_space::hash<stream_data>()(data);}
    };
    struct frame__new_token {
    unsigned long long length;
    stream_data data;
        size_t __hash() const { return hash_space::hash<unsigned long long>()(length)+hash_space::hash<stream_data>()(data);}
    };
    struct frame__max_data {
    unsigned long long pos;
        size_t __hash() const { return hash_space::hash<unsigned long long>()(pos);}
    };
    struct frame__stream_blocked {
    unsigned id;
    unsigned long long pos;
        size_t __hash() const { return hash_space::hash<unsigned>()(id)+hash_space::hash<unsigned long long>()(pos);}
    };
    struct frame__stop_sending {
    unsigned id;
    unsigned err_code;
        size_t __hash() const { return hash_space::hash<unsigned>()(id)+hash_space::hash<unsigned>()(err_code);}
    };
    struct frame__blocked {
    unsigned long long pos;
        size_t __hash() const { return hash_space::hash<unsigned long long>()(pos);}
    };
    struct frame__retire_connection_id {
    unsigned seq_num;
        size_t __hash() const { return hash_space::hash<unsigned>()(seq_num);}
    };
    class frame__arr : public std::vector<frame>{
        public: size_t __hash() const { return hash_space::hash<std::vector<frame> >()(*this);};
    };
class transport_parameter{
public:
    struct wrap {
    
        virtual wrap *dup() = 0;
        virtual bool deref() = 0;
        virtual ~wrap() {}
    };
    
    template <typename T> struct twrap : public wrap {
    
        unsigned refs;
    
        T item;
    
        twrap(const T &item) : refs(1), item(item) {}
    
        virtual wrap *dup() {refs++; return this;}
    
        virtual bool deref() {return (--refs) != 0;}
    
    };
    
    int tag;
    
    wrap *ptr;
    
    transport_parameter(){
    tag=-1;
    ptr=0;
    }
    
    transport_parameter(int tag,wrap *ptr) : tag(tag),ptr(ptr) {}
    
    transport_parameter(const transport_parameter&other){
        tag=other.tag;
        ptr = other.ptr ? other.ptr->dup() : 0;
    };
    
    transport_parameter& operator=(const transport_parameter&other){
        tag=other.tag;
        ptr = other.ptr ? other.ptr->dup() : 0;
        return *this;
    };
    
    ~transport_parameter(){if(ptr){if (!ptr->deref()) delete ptr;}}
    
    static int temp_counter;
    
    static void prepare() {temp_counter = 0;}
    
    static void cleanup() {}
    
    size_t __hash() const {
    
        switch(tag) {
    
            case 0: return 0 + hash_space::hash<quic_server_test_stream::initial_max_stream_data_bidi_local>()(quic_server_test_stream::transport_parameter::unwrap< quic_server_test_stream::initial_max_stream_data_bidi_local >((*this)));
    
            case 1: return 1 + hash_space::hash<quic_server_test_stream::initial_max_data>()(quic_server_test_stream::transport_parameter::unwrap< quic_server_test_stream::initial_max_data >((*this)));
    
            case 2: return 2 + hash_space::hash<quic_server_test_stream::initial_max_stream_id_bidi>()(quic_server_test_stream::transport_parameter::unwrap< quic_server_test_stream::initial_max_stream_id_bidi >((*this)));
    
            case 3: return 3 + hash_space::hash<quic_server_test_stream::idle_timeout>()(quic_server_test_stream::transport_parameter::unwrap< quic_server_test_stream::idle_timeout >((*this)));
    
            case 4: return 4 + hash_space::hash<quic_server_test_stream::preferred_address>()(quic_server_test_stream::transport_parameter::unwrap< quic_server_test_stream::preferred_address >((*this)));
    
            case 5: return 5 + hash_space::hash<quic_server_test_stream::max_packet_size>()(quic_server_test_stream::transport_parameter::unwrap< quic_server_test_stream::max_packet_size >((*this)));
    
            case 6: return 6 + hash_space::hash<quic_server_test_stream::stateless_reset_token>()(quic_server_test_stream::transport_parameter::unwrap< quic_server_test_stream::stateless_reset_token >((*this)));
    
            case 7: return 7 + hash_space::hash<quic_server_test_stream::ack_delay_exponent>()(quic_server_test_stream::transport_parameter::unwrap< quic_server_test_stream::ack_delay_exponent >((*this)));
    
            case 8: return 8 + hash_space::hash<quic_server_test_stream::initial_max_stream_id_uni>()(quic_server_test_stream::transport_parameter::unwrap< quic_server_test_stream::initial_max_stream_id_uni >((*this)));
    
            case 9: return 9 + hash_space::hash<quic_server_test_stream::disable_migration>()(quic_server_test_stream::transport_parameter::unwrap< quic_server_test_stream::disable_migration >((*this)));
    
            case 10: return 10 + hash_space::hash<quic_server_test_stream::initial_max_stream_data_bidi_remote>()(quic_server_test_stream::transport_parameter::unwrap< quic_server_test_stream::initial_max_stream_data_bidi_remote >((*this)));
    
            case 11: return 11 + hash_space::hash<quic_server_test_stream::initial_max_stream_data_uni>()(quic_server_test_stream::transport_parameter::unwrap< quic_server_test_stream::initial_max_stream_data_uni >((*this)));
    
            case 12: return 12 + hash_space::hash<quic_server_test_stream::max_ack_delay>()(quic_server_test_stream::transport_parameter::unwrap< quic_server_test_stream::max_ack_delay >((*this)));
    
            case 13: return 13 + hash_space::hash<quic_server_test_stream::active_connection_id_limit>()(quic_server_test_stream::transport_parameter::unwrap< quic_server_test_stream::active_connection_id_limit >((*this)));
    
        }
    
        return 0;
    
    }
    
    template <typename T> static const T &unwrap(const transport_parameter &x) {
    
        return ((static_cast<const twrap<T> *>(x.ptr))->item);
    
    }
    
    template <typename T> static T &unwrap(transport_parameter &x) {
    
         twrap<T> *p = static_cast<twrap<T> *>(x.ptr);
    
         if (p->refs > 1) {
    
             p = new twrap<T> (p->item);
    
         }
    
         return ((static_cast<twrap<T> *>(p))->item);
    
    }
    
};    struct initial_max_stream_data_bidi_local {
    unsigned long long stream_pos_32;
        size_t __hash() const { return hash_space::hash<unsigned long long>()(stream_pos_32);}
    };
    struct initial_max_data {
    unsigned long long stream_pos_32;
        size_t __hash() const { return hash_space::hash<unsigned long long>()(stream_pos_32);}
    };
    struct initial_max_stream_id_bidi {
    unsigned stream_id_16;
        size_t __hash() const { return hash_space::hash<unsigned>()(stream_id_16);}
    };
    struct idle_timeout {
    int seconds_16;
        size_t __hash() const { return hash_space::hash<int>()(seconds_16);}
    };
    struct preferred_address {
        size_t __hash() const { return 0;}
    };
    struct max_packet_size {
    unsigned long long stream_pos_16;
        size_t __hash() const { return hash_space::hash<unsigned long long>()(stream_pos_16);}
    };
    struct stateless_reset_token {
    stream_data data_8;
        size_t __hash() const { return hash_space::hash<stream_data>()(data_8);}
    };
    struct ack_delay_exponent {
    int exponent_8;
        size_t __hash() const { return hash_space::hash<int>()(exponent_8);}
    };
    struct initial_max_stream_id_uni {
    unsigned stream_id_16;
        size_t __hash() const { return hash_space::hash<unsigned>()(stream_id_16);}
    };
    struct disable_migration {
        size_t __hash() const { return 0;}
    };
    struct initial_max_stream_data_bidi_remote {
    unsigned long long stream_pos_32;
        size_t __hash() const { return hash_space::hash<unsigned long long>()(stream_pos_32);}
    };
    struct initial_max_stream_data_uni {
    unsigned long long stream_pos_32;
        size_t __hash() const { return hash_space::hash<unsigned long long>()(stream_pos_32);}
    };
    struct max_ack_delay {
    int exponent_8;
        size_t __hash() const { return hash_space::hash<int>()(exponent_8);}
    };
    struct active_connection_id_limit {
    unsigned long long stream_pos_32;
        size_t __hash() const { return hash_space::hash<unsigned long long>()(stream_pos_32);}
    };
    struct trans_params_struct {
    bool initial_max_stream_data_bidi_local__is_set;
    initial_max_stream_data_bidi_local initial_max_stream_data_bidi_local__value;
    bool initial_max_data__is_set;
    initial_max_data initial_max_data__value;
    bool initial_max_stream_id_bidi__is_set;
    initial_max_stream_id_bidi initial_max_stream_id_bidi__value;
    bool idle_timeout__is_set;
    idle_timeout idle_timeout__value;
    bool preferred_address__is_set;
    preferred_address preferred_address__value;
    bool max_packet_size__is_set;
    max_packet_size max_packet_size__value;
    bool stateless_reset_token__is_set;
    stateless_reset_token stateless_reset_token__value;
    bool ack_delay_exponent__is_set;
    ack_delay_exponent ack_delay_exponent__value;
    bool initial_max_stream_id_uni__is_set;
    initial_max_stream_id_uni initial_max_stream_id_uni__value;
    bool disable_migration__is_set;
    disable_migration disable_migration__value;
    bool initial_max_stream_data_bidi_remote__is_set;
    initial_max_stream_data_bidi_remote initial_max_stream_data_bidi_remote__value;
    bool initial_max_stream_data_uni__is_set;
    initial_max_stream_data_uni initial_max_stream_data_uni__value;
    bool max_ack_delay__is_set;
    max_ack_delay max_ack_delay__value;
    bool active_connection_id_limit__is_set;
    active_connection_id_limit active_connection_id_limit__value;
        size_t __hash() const { return hash_space::hash<bool>()(initial_max_stream_data_bidi_local__is_set)+hash_space::hash<initial_max_stream_data_bidi_local>()(initial_max_stream_data_bidi_local__value)+hash_space::hash<bool>()(initial_max_data__is_set)+hash_space::hash<initial_max_data>()(initial_max_data__value)+hash_space::hash<bool>()(initial_max_stream_id_bidi__is_set)+hash_space::hash<initial_max_stream_id_bidi>()(initial_max_stream_id_bidi__value)+hash_space::hash<bool>()(idle_timeout__is_set)+hash_space::hash<idle_timeout>()(idle_timeout__value)+hash_space::hash<bool>()(preferred_address__is_set)+hash_space::hash<preferred_address>()(preferred_address__value)+hash_space::hash<bool>()(max_packet_size__is_set)+hash_space::hash<max_packet_size>()(max_packet_size__value)+hash_space::hash<bool>()(stateless_reset_token__is_set)+hash_space::hash<stateless_reset_token>()(stateless_reset_token__value)+hash_space::hash<bool>()(ack_delay_exponent__is_set)+hash_space::hash<ack_delay_exponent>()(ack_delay_exponent__value)+hash_space::hash<bool>()(initial_max_stream_id_uni__is_set)+hash_space::hash<initial_max_stream_id_uni>()(initial_max_stream_id_uni__value)+hash_space::hash<bool>()(disable_migration__is_set)+hash_space::hash<disable_migration>()(disable_migration__value)+hash_space::hash<bool>()(initial_max_stream_data_bidi_remote__is_set)+hash_space::hash<initial_max_stream_data_bidi_remote>()(initial_max_stream_data_bidi_remote__value)+hash_space::hash<bool>()(initial_max_stream_data_uni__is_set)+hash_space::hash<initial_max_stream_data_uni>()(initial_max_stream_data_uni__value)+hash_space::hash<bool>()(max_ack_delay__is_set)+hash_space::hash<max_ack_delay>()(max_ack_delay__value)+hash_space::hash<bool>()(active_connection_id_limit__is_set)+hash_space::hash<active_connection_id_limit>()(active_connection_id_limit__value);}
    };
    class vector__transport_parameter__ : public std::vector<transport_parameter>{
        public: size_t __hash() const { return hash_space::hash<std::vector<transport_parameter> >()(*this);};
    };
    struct quic_transport_parameters {
    vector__transport_parameter__ transport_parameters;
        size_t __hash() const { return hash_space::hash<vector__transport_parameter__>()(transport_parameters);}
    };
    struct quic_packet {
    quic_packet_type ptype;
    unsigned pversion;
    cid dst_cid;
    cid src_cid;
    stream_data token;
    unsigned seq_num;
    frame__arr payload;
        size_t __hash() const { return hash_space::hash<int>()(ptype)+hash_space::hash<unsigned>()(pversion)+hash_space::hash<cid>()(dst_cid)+hash_space::hash<cid>()(src_cid)+hash_space::hash<stream_data>()(token)+hash_space::hash<unsigned>()(seq_num)+hash_space::hash<frame__arr>()(payload);}
    };
    class quic_packet__arr : public std::vector<quic_packet>{
        public: size_t __hash() const { return hash_space::hash<std::vector<quic_packet> >()(*this);};
    };
    class quic_packet__retired_cids : public std::vector<unsigned>{
        public: size_t __hash() const { return hash_space::hash<std::vector<unsigned> >()(*this);};
    };
    class prot__arr : public std::vector<stream_data>{
        public: size_t __hash() const { return hash_space::hash<std::vector<stream_data> >()(*this);};
    };
    struct prot__header_info {
    bool hdr_long;
    unsigned hdr_type;
    cid dcid;
    cid scid;
    unsigned long long payload_length;
    unsigned long long payload_length_pos;
    unsigned long long pkt_num_pos;
        size_t __hash() const { return hash_space::hash<bool>()(hdr_long)+hash_space::hash<unsigned>()(hdr_type)+hash_space::hash<cid>()(dcid)+hash_space::hash<cid>()(scid)+hash_space::hash<unsigned long long>()(payload_length)+hash_space::hash<unsigned long long>()(payload_length_pos)+hash_space::hash<unsigned long long>()(pkt_num_pos);}
    };
    struct tls_api__upper__decrypt_result {
    bool ok;
    stream_data data;
        size_t __hash() const { return hash_space::hash<bool>()(ok)+hash_space::hash<stream_data>()(data);}
    };
    enum endpoint_id{endpoint_id__client,endpoint_id__client_alt,endpoint_id__server};
    class tls_extensions : public std::vector<tls__extension>{
        public: size_t __hash() const { return hash_space::hash<std::vector<tls__extension> >()(*this);};
    };
struct __tup__cid__unsigned {
    cid arg0;
    unsigned arg1;
__tup__cid__unsigned(){}__tup__cid__unsigned(const cid &arg0,const unsigned &arg1) : arg0(arg0),arg1(arg1){}
        size_t __hash() const { return hash_space::hash<cid>()(arg0)+hash_space::hash<unsigned>()(arg1);}
};
struct __tup__cid__stream_kind {
    cid arg0;
    stream_kind arg1;
__tup__cid__stream_kind(){}__tup__cid__stream_kind(const cid &arg0,const stream_kind &arg1) : arg0(arg0),arg1(arg1){}
        size_t __hash() const { return hash_space::hash<cid>()(arg0)+hash_space::hash<int>()(arg1);}
};
struct __tup__cid__quic_packet_type__unsigned {
    cid arg0;
    quic_packet_type arg1;
    unsigned arg2;
__tup__cid__quic_packet_type__unsigned(){}__tup__cid__quic_packet_type__unsigned(const cid &arg0,const quic_packet_type &arg1,const unsigned &arg2) : arg0(arg0),arg1(arg1),arg2(arg2){}
        size_t __hash() const { return hash_space::hash<cid>()(arg0)+hash_space::hash<int>()(arg1)+hash_space::hash<unsigned>()(arg2);}
};
struct __tup__cid__quic_packet_type {
    cid arg0;
    quic_packet_type arg1;
__tup__cid__quic_packet_type(){}__tup__cid__quic_packet_type(const cid &arg0,const quic_packet_type &arg1) : arg0(arg0),arg1(arg1){}
        size_t __hash() const { return hash_space::hash<cid>()(arg0)+hash_space::hash<int>()(arg1);}
};
struct __tup__ip__endpoint__cid {
    ip__endpoint arg0;
    cid arg1;
__tup__ip__endpoint__cid(){}__tup__ip__endpoint__cid(const ip__endpoint &arg0,const cid &arg1) : arg0(arg0),arg1(arg1){}
        size_t __hash() const { return hash_space::hash<ip__endpoint>()(arg0)+hash_space::hash<cid>()(arg1);}
};
struct __tup__cid__quic_packet_type__unsigned_long_long {
    cid arg0;
    quic_packet_type arg1;
    unsigned long long arg2;
__tup__cid__quic_packet_type__unsigned_long_long(){}__tup__cid__quic_packet_type__unsigned_long_long(const cid &arg0,const quic_packet_type &arg1,const unsigned long long &arg2) : arg0(arg0),arg1(arg1),arg2(arg2){}
        size_t __hash() const { return hash_space::hash<cid>()(arg0)+hash_space::hash<int>()(arg1)+hash_space::hash<unsigned long long>()(arg2);}
};
struct __tup__cid__ip__endpoint {
    cid arg0;
    ip__endpoint arg1;
__tup__cid__ip__endpoint(){}__tup__cid__ip__endpoint(const cid &arg0,const ip__endpoint &arg1) : arg0(arg0),arg1(arg1){}
        size_t __hash() const { return hash_space::hash<cid>()(arg0)+hash_space::hash<ip__endpoint>()(arg1);}
};
struct __tup__ip__endpoint__ip__endpoint__cid {
    ip__endpoint arg0;
    ip__endpoint arg1;
    cid arg2;
__tup__ip__endpoint__ip__endpoint__cid(){}__tup__ip__endpoint__ip__endpoint__cid(const ip__endpoint &arg0,const ip__endpoint &arg1,const cid &arg2) : arg0(arg0),arg1(arg1),arg2(arg2){}
        size_t __hash() const { return hash_space::hash<ip__endpoint>()(arg0)+hash_space::hash<ip__endpoint>()(arg1)+hash_space::hash<cid>()(arg2);}
};
struct __tup__cid__stream_data {
    cid arg0;
    stream_data arg1;
__tup__cid__stream_data(){}__tup__cid__stream_data(const cid &arg0,const stream_data &arg1) : arg0(arg0),arg1(arg1){}
        size_t __hash() const { return hash_space::hash<cid>()(arg0)+hash_space::hash<stream_data>()(arg1);}
};

class hash____tup__cid__unsigned {
    public:
        size_t operator()(const quic_server_test_stream::__tup__cid__unsigned &__s) const {
            return hash_space::hash<cid>()(__s.arg0)+hash_space::hash<unsigned>()(__s.arg1);
        }
    };

class hash____tup__cid__stream_kind {
    public:
        size_t operator()(const quic_server_test_stream::__tup__cid__stream_kind &__s) const {
            return hash_space::hash<cid>()(__s.arg0)+hash_space::hash<int>()(__s.arg1);
        }
    };

class hash____tup__cid__quic_packet_type__unsigned {
    public:
        size_t operator()(const quic_server_test_stream::__tup__cid__quic_packet_type__unsigned &__s) const {
            return hash_space::hash<cid>()(__s.arg0)+hash_space::hash<int>()(__s.arg1)+hash_space::hash<unsigned>()(__s.arg2);
        }
    };

class hash____tup__cid__quic_packet_type {
    public:
        size_t operator()(const quic_server_test_stream::__tup__cid__quic_packet_type &__s) const {
            return hash_space::hash<cid>()(__s.arg0)+hash_space::hash<int>()(__s.arg1);
        }
    };

class hash____tup__ip__endpoint__cid {
    public:
        size_t operator()(const quic_server_test_stream::__tup__ip__endpoint__cid &__s) const {
            return hash_space::hash<ip__endpoint>()(__s.arg0)+hash_space::hash<cid>()(__s.arg1);
        }
    };

class hash____tup__cid__quic_packet_type__unsigned_long_long {
    public:
        size_t operator()(const quic_server_test_stream::__tup__cid__quic_packet_type__unsigned_long_long &__s) const {
            return hash_space::hash<cid>()(__s.arg0)+hash_space::hash<int>()(__s.arg1)+hash_space::hash<unsigned long long>()(__s.arg2);
        }
    };

class hash____tup__cid__ip__endpoint {
    public:
        size_t operator()(const quic_server_test_stream::__tup__cid__ip__endpoint &__s) const {
            return hash_space::hash<cid>()(__s.arg0)+hash_space::hash<ip__endpoint>()(__s.arg1);
        }
    };

class hash____tup__ip__endpoint__ip__endpoint__cid {
    public:
        size_t operator()(const quic_server_test_stream::__tup__ip__endpoint__ip__endpoint__cid &__s) const {
            return hash_space::hash<ip__endpoint>()(__s.arg0)+hash_space::hash<ip__endpoint>()(__s.arg1)+hash_space::hash<cid>()(__s.arg2);
        }
    };

class hash____tup__cid__stream_data {
    public:
        size_t operator()(const quic_server_test_stream::__tup__cid__stream_data &__s) const {
            return hash_space::hash<cid>()(__s.arg0)+hash_space::hash<stream_data>()(__s.arg1);
        }
    };
    hash_thunk<__tup__cid__unsigned,unsigned long long> stream_length;
    hash_thunk<__tup__cid__stream_kind,bool> max_stream_set;
    ip__endpoint client_alt;
    hash_thunk<cid,bool> conn_draining;
    hash_thunk<cid,frame__arr> queued_frames;
    hash_thunk<__tup__cid__quic_packet_type__unsigned,bool> acked_pkt;
    hash_thunk<__tup__cid__quic_packet_type,unsigned long long> crypto_handler_pos;
    hash_thunk<__tup__cid__quic_packet_type,unsigned> max_acked;
    hash_thunk<__tup__ip__endpoint__cid,quic_packet_type> conn_enc_level;
    hash_thunk<cid,bool> queued_non_ack;
    unsigned server_addr;
    hash_thunk<__tup__cid__unsigned,unsigned long long> max_stream_data_val;
    hash_thunk<__tup__cid__quic_packet_type,unsigned long long> crypto_pos;
    hash_thunk<cid,bool> queued_non_probing;
    hash_thunk<__tup__cid__quic_packet_type__unsigned_long_long,bool> crypto_data_present;
    unsigned current_stream;
    hash_thunk<cid,bool> queued_close;
    hash_thunk<__tup__cid__ip__endpoint,bool> hi_non_probing_endpoint;
    hash_thunk<__tup__cid__unsigned,bool> stream_seen;
    hash_thunk<__tup__cid__unsigned,unsigned long long> stream_app_data_end;
    unsigned client_addr;
    hash_thunk<cid,unsigned long long> max_data_val;
    hash_thunk<cid,bool> max_data_set;
    hash_thunk<__tup__cid__unsigned,bool> stream_finished;
    unsigned long long client__tls_id;
    hash_thunk<__tup__cid__quic_packet_type__unsigned,bool> pkt_has_close;
    hash_thunk<cid,bool> trans_params_set;
    hash_thunk<cid,bool> cid_mapped;
    hash_thunk<__tup__cid__quic_packet_type,unsigned> last_pkt_num;
    unsigned server_port;
    stream_data http_request;
    hash_thunk<cid,unsigned long long> num_queued_frames;
    unsigned client_port;
    ip__endpoint client__ep;
    unsigned long long max_stream_data;
    bool force_new_ack;
    hash_thunk<__tup__cid__quic_packet_type,unsigned long long> crypto_data_end;
    hash_thunk<__tup__cid__unsigned,stream_data> stream_app_data;
    ip__endpoint server__ep;
    hash_thunk<__tup__cid__quic_packet_type,stream_data> crypto_data;
    hash_thunk<__tup__cid__unsigned,unsigned long long> stream_app_pos;
    hash_thunk<cid,quic_packet_type> queued_level;
    hash_thunk<cid,bool> draining_pkt_sent;
    hash_thunk<cid,unsigned long long> conn_total_data;
    hash_thunk<cid,bool> established_handshake_keys;
    int sock;
    quic_packet_type server__enc_level;
    hash_thunk<__tup__ip__endpoint__ip__endpoint__cid,bool> conn_requested;
    hash_thunk<cid,bool> used_cid;
    hash_thunk<cid,bool> conn_closed;
    hash_thunk<__tup__cid__unsigned,bool> stream_reset;
    cid the_cid;
    quic_packet_type client__enc_level;
    unsigned client_port_alt;
    hash_thunk<cid,bool> queued_ack_eliciting;
    unsigned long long server__tls_id;
    hash_thunk<__tup__cid__quic_packet_type,unsigned long long> crypto_length;
    hash_thunk<cid,bool> is_client;
    hash_thunk<cid,unsigned> max_seq_num;
    hash_thunk<__tup__cid__unsigned,bool> stream_app_data_finished;
    int sock_alt;
    hash_thunk<cid,cid> nonce_cid;
    hash_thunk<cid,bool> connected;
    hash_thunk<cid,bool> conn_seen;
    hash_thunk<cid,unsigned> ack_credit;
    cid server_cid;
    hash_thunk<cid,trans_params_struct> trans_params;
    bool _generating;
    hash_thunk<__tup__cid__unsigned,cid> seqnum_to_cid;
    hash_thunk<cid,cid> cid_to_aid;
    hash_thunk<cid,unsigned> hi_non_probing;
    hash_thunk<cid,cid> connected_to;
    hash_thunk<__tup__cid__unsigned,bool> max_stream_data_set;
    hash_thunk<__tup__cid__stream_kind,unsigned> max_stream;
    hash_thunk<__tup__cid__stream_data,bool> path_challenge_pending;
    hash_thunk<__tup__cid__quic_packet_type__unsigned,bool> sent_pkt;
    hash_thunk<cid,bool> established_1rtt_keys;
    long long __CARD__stream_pos;
    long long __CARD__tls__gmt;
    long long __CARD__microsecs;
    long long __CARD__vector__tls__extension____domain;
    long long __CARD__vector__tls__handshake____domain;
    long long __CARD__tls__cipher_suite;
    long long __CARD__byte;
    long long __CARD__vector__transport_parameter____domain;
    long long __CARD__cid_length;
    long long __CARD__tls__message_type;
    long long __CARD__cid_seq;
    long long __CARD__tls__handshakes__domain;
    long long __CARD__quic_packet__idx;
    long long __CARD__version;
    long long __CARD__tls__compression_method;
    long long __CARD__tls_api__lower__level;
    long long __CARD__frame__idx;
    long long __CARD__frame__ack__block__idx;
    long long __CARD__net__socket;
    long long __CARD__tls__protocol_version;
    long long __CARD__vector__tls__cipher_suite____domain;
    long long __CARD__tls_api__upper__level;
    long long __CARD__ip__addr;
    long long __CARD__bit;
    long long __CARD__tls_extensions__domain;
    long long __CARD__prot__idx;
    long long __CARD__tls_api__id;
    long long __CARD__cid;
    long long __CARD__vector__tls__compression_method____domain;
    long long __CARD__pkt_num;
    long long __CARD__reset_token;
    long long __CARD__tls__extension_type;
    long long __CARD__stream_id;
    long long __CARD__type_bits;
    long long __CARD__error_code;
    long long __CARD__ip__port;
    virtual unsigned bit__one();
    virtual unsigned long long stream_data__begin(const stream_data& A);
    virtual stream_kind get_stream_kind(unsigned S);
    virtual role get_stream_role(unsigned S);
    virtual unsigned long long vector__tls__extension____begin(const vector__tls__extension__& A);
    virtual unsigned long long tls__handshakes__begin(const tls__handshakes& A);
    virtual unsigned long long frame__arr__begin(const frame__arr& A);
    virtual unsigned long long vector__transport_parameter____begin(const vector__transport_parameter__& A);
    virtual unsigned long long prot__arr__begin(const prot__arr& A);
    virtual unsigned stream_data__value(const stream_data& a, unsigned long long i);
    virtual unsigned long long stream_data__end(const stream_data& a);
    virtual stream_data stream_data__segment(const stream_data& a, unsigned long long lo, unsigned long long hi);
    virtual unsigned vector__tls__cipher_suite____value(const vector__tls__cipher_suite__& a, unsigned long long i);
    virtual unsigned long long vector__tls__cipher_suite____end(const vector__tls__cipher_suite__& a);
    virtual unsigned vector__tls__compression_method____value(const vector__tls__compression_method__& a, unsigned long long i);
    virtual unsigned long long vector__tls__compression_method____end(const vector__tls__compression_method__& a);
    virtual quic_server_test_stream::tls__extension vector__tls__extension____value(const vector__tls__extension__& a, unsigned long long i);
    virtual unsigned long long vector__tls__extension____end(const vector__tls__extension__& a);
    virtual quic_server_test_stream::tls__handshake vector__tls__handshake____value(const vector__tls__handshake__& a, unsigned long long i);
    virtual unsigned long long vector__tls__handshake____end(const vector__tls__handshake__& a);
    virtual quic_server_test_stream::tls__handshake tls__handshakes__value(const tls__handshakes& a, unsigned long long i);
    virtual unsigned long long tls__handshakes__end(const tls__handshakes& a);
    virtual frame__ack__block frame__ack__block__arr__value(const frame__ack__block__arr& a, unsigned long long i);
    virtual unsigned long long frame__ack__block__arr__end(const frame__ack__block__arr& a);
    virtual quic_server_test_stream::frame frame__arr__value(const frame__arr& a, unsigned long long i);
    virtual unsigned long long frame__arr__end(const frame__arr& a);
    virtual quic_server_test_stream::transport_parameter vector__transport_parameter____value(const vector__transport_parameter__& a, unsigned long long i);
    virtual unsigned long long vector__transport_parameter____end(const vector__transport_parameter__& a);
    virtual quic_packet quic_packet__arr__value(const quic_packet__arr& a, unsigned long long i);
    virtual unsigned long long quic_packet__arr__end(const quic_packet__arr& a);
    virtual unsigned quic_packet__retired_cids__value(const quic_packet__retired_cids& a, unsigned long long i);
    virtual unsigned long long quic_packet__retired_cids__end(const quic_packet__retired_cids& a);
    virtual stream_data prot__arr__value(const prot__arr& a, unsigned long long i);
    virtual unsigned long long prot__arr__end(const prot__arr& a);
    virtual quic_server_test_stream::tls__extension tls_extensions__value(const tls_extensions& a, unsigned long long i);
    virtual unsigned long long tls_extensions__end(const tls_extensions& a);

    class tls_deser;
    class tls_ser;
    class tls_ser_server;


    typedef ivy_binary_ser std_serializer;
    typedef ivy_binary_deser std_deserializer;


    class quic_deser;


    class quic_ser;


    class quic_prot_ser;


    class quic_prot_deser;


    hash_space::hash_map<unsigned long long,picotls_connection *> tls_api__upper__impl__cid_map; // maps cid's to connections
    tls_callbacks *tls_api__upper__impl__cb;             // the callbacks to ivy


	udp_callbacks *net__impl__cb[3];             // the callbacks to ivy

                int http_request_file__fildes;
            quic_server_test_stream(quic_server_test_stream::cid the_cid, unsigned client_addr, unsigned server_addr, unsigned server_port, quic_server_test_stream::cid server_cid, unsigned client_port, unsigned client_port_alt, unsigned long long max_stream_data);
    virtual void ext__frame__connection_close__handle(const frame__connection_close& f, quic_server_test_stream::cid scid, quic_server_test_stream::cid dcid, quic_packet_type e);
    virtual int ext__prot__get_level(const stream_data& pkt);
    virtual void ext__active_connection_id_limit__set(const active_connection_id_limit& p, trans_params_struct& s);
    virtual int ext__net__impl__open(endpoint_id prm__V0, const ip__endpoint& addr);
    virtual void ext__initial_max_stream_id_bidi__set(const initial_max_stream_id_bidi& p, trans_params_struct& s);
    virtual frame__arr ext__frame__arr__empty();
    virtual void ext__tls_api__upper__create(unsigned long long c, bool is_server, const tls_extensions& e);
    virtual unsigned long long ext__tls_api__upper__iv_size(unsigned long long c, int l);
    virtual void ext___finalize();
    virtual void ext__max_packet_size__set(const max_packet_size& p, trans_params_struct& s);
    virtual void tls_api__upper__keys_established(unsigned long long c, int lev);
    virtual void ext__prot__arr__append(prot__arr& a, const stream_data& v);
    virtual unsigned long long ext__prot__idx__next(unsigned long long x);
    virtual quic_server_test_stream::cid ext__prot__bytes_to_cid(const stream_data& bytes);
    virtual void ext__frame__stream__handle(frame__stream f, quic_server_test_stream::cid scid, quic_server_test_stream::cid dcid, quic_packet_type e);
    virtual void ext__frame__retire_connection_id__handle(const frame__retire_connection_id& f, quic_server_test_stream::cid scid, quic_server_test_stream::cid dcid, quic_packet_type e);
    virtual void ext__enqueue_frame(quic_server_test_stream::cid scid, quic_server_test_stream::frame f, quic_packet_type e, bool probing);
    virtual void imp__recv_packet(const ip__endpoint& src, const ip__endpoint& dst, const quic_packet& pkt);
    virtual stream_data ext__tls_api__upper__encrypt_aead(unsigned long long c, int l, const stream_data& clear, unsigned seq, const stream_data& ad);
    virtual stream_data ext__http_request_file__read();
    virtual stream_data ext__prot__to_var_int_16(unsigned long long val);
    virtual ip__endpoint ext__tls_id_to_dst(unsigned long long tls_id);
    virtual void ext__frame__max_data__handle(const frame__max_data& f, quic_server_test_stream::cid scid, quic_server_test_stream::cid dcid, quic_packet_type e);
    virtual void ext__tls_extensions__append(tls_extensions& a, quic_server_test_stream::tls__extension v);
    virtual void imp__undecryptable_packet_event(const ip__endpoint& src, const ip__endpoint& dst, const stream_data& pkt);
    virtual int ext__net__open(endpoint_id me, const ip__endpoint& addr);
    virtual void ext__map_cids(quic_server_test_stream::cid dcid, quic_server_test_stream::cid scid);
    virtual void ext__frame__stop_sending__handle(const frame__stop_sending& f, quic_server_test_stream::cid scid, quic_server_test_stream::cid dcid, quic_packet_type e);
    virtual void ext__handle_tls_extensions(const ip__endpoint& src, const ip__endpoint& dst, quic_server_test_stream::cid scid, const vector__tls__extension__& exts, bool is_client_hello);
    virtual void ext__initial_max_data__set(const initial_max_data& p, trans_params_struct& s);
    virtual void ext__net__send(endpoint_id me, int s, const ip__endpoint& dst, const prot__arr& x);
    virtual unsigned ext__prot__get_pnum(const stream_data& pkt, unsigned long long pnum_pos, unsigned long long pnum_len);
    virtual void ext__tls__handshake_event(const ip__endpoint& src, const ip__endpoint& dst, quic_server_test_stream::tls__handshake h);
    virtual stream_data ext__stream_data__empty();
    virtual void ext__stream_data__append(stream_data& a, unsigned v);
    virtual tls__handshake_parser__result ext__tls__handshake_parser__deserialize(const stream_data& x, unsigned long long pos);
    virtual void ext__tls_client_initial_request(const ip__endpoint& src, const ip__endpoint& dst, quic_server_test_stream::cid nonce);
    virtual void ext__vector__transport_parameter____append(vector__transport_parameter__& a, quic_server_test_stream::transport_parameter v);
    virtual void ext__stream_data__extend(stream_data& a, const stream_data& b);
    virtual void ext__frame__path_challenge__handle(const frame__path_challenge& f, quic_server_test_stream::cid scid, quic_server_test_stream::cid dcid, quic_packet_type e);
    virtual void ext__frame__stream_id_blocked__handle(const frame__stream_id_blocked& f, quic_server_test_stream::cid scid, quic_server_test_stream::cid dcid, quic_packet_type e);
    virtual void ext__initial_max_stream_data_bidi_local__set(const initial_max_stream_data_bidi_local& p, trans_params_struct& s);
    virtual void ext__frame__path_response__handle(frame__path_response f, quic_server_test_stream::cid scid, quic_server_test_stream::cid dcid, quic_packet_type e);
    virtual void __init();
    virtual unsigned long long ext__prot__get_var_int_len(const stream_data& pkt, unsigned long long pos);
    virtual quic_packet_type ext__packet_encryption_level(const prot__header_info& h);
    virtual void tls_api__upper__alert(unsigned long long c, const stream_data& data);
    virtual unsigned long long ext__max_additional_data(quic_server_test_stream::cid dcid);
    virtual void ext__frame__handle(quic_server_test_stream::frame f, quic_server_test_stream::cid scid, quic_server_test_stream::cid dcid, quic_packet_type e);
    virtual void ext__tls_api__upper__set_initial_keys(unsigned long long c, const stream_data& salt, const stream_data& ikm);
    virtual ip__endpoint ext__socket_endpoint(endpoint_id host, int s);
    virtual void ext__frame__max_stream_data__handle(const frame__max_stream_data& f, quic_server_test_stream::cid scid, quic_server_test_stream::cid dcid, quic_packet_type e);
    virtual void ext__frame__rst_stream__handle(const frame__rst_stream& f, quic_server_test_stream::cid scid, quic_server_test_stream::cid dcid, quic_packet_type e);
    virtual void net__impl__handle_recv(endpoint_id prm__V0, int s, const ip__endpoint& src, const prot__arr& x);
    virtual void ext__client_send_event(ip__endpoint src, ip__endpoint dst, quic_server_test_stream::cid dcid, unsigned s, unsigned long long end);
    virtual void ext__frame__blocked__handle(const frame__blocked& f, quic_server_test_stream::cid scid, quic_server_test_stream::cid dcid, quic_packet_type e);
    virtual void ext__frame__max_stream_id__handle(const frame__max_stream_id& f, quic_server_test_stream::cid scid, quic_server_test_stream::cid dcid, quic_packet_type e);
    virtual prot__arr ext__prot__arr__empty();
    virtual void ext__preferred_address__set(const preferred_address& p, trans_params_struct& s);
    virtual void ext__disable_migration__set(const disable_migration& p, trans_params_struct& s);
    virtual void ext__prot__correct_pnum(unsigned last, unsigned& pnum, unsigned long long pnum_len);
    virtual void ext__frame__ping__handle(const frame__ping& f, quic_server_test_stream::cid scid, quic_server_test_stream::cid dcid, quic_packet_type e);
    virtual unsigned ext__reference_pkt_num(const stream_data& spkt);
    virtual bool ext__stream_id_allowed(quic_server_test_stream::cid dcid, unsigned id);
    virtual tls_extensions ext__tls_extensions__empty();
    virtual void ext__ack_delay_exponent__set(const ack_delay_exponent& p, trans_params_struct& s);
    virtual bool ext__dst_is_generated(const ip__endpoint& dst);
    virtual void ext__recv_packet(const ip__endpoint& src, const ip__endpoint& dst, const quic_packet& pkt);
    virtual unsigned long long ext__dst_tls_id(const ip__endpoint& dst);
    virtual unsigned long long ext__frame__ack__block__idx__next(unsigned long long x);
    virtual void ext__transport_parameter__set(quic_server_test_stream::transport_parameter p, trans_params_struct& s);
    virtual unsigned long long ext__vector__tls__extension____domain__next(unsigned long long x);
    virtual void ext__frame__new_connection_id__handle(const frame__new_connection_id& f, quic_server_test_stream::cid scid, quic_server_test_stream::cid dcid, quic_packet_type e);
    virtual void ext__handle_tls_handshake(const ip__endpoint& src, const ip__endpoint& dst, quic_server_test_stream::cid scid, quic_server_test_stream::cid dcid, quic_server_test_stream::tls__handshake hs);
    virtual void ext__prot__encrypt(unsigned long long c, unsigned seq, stream_data& pkt);
    virtual void ext__tls_recv_event(ip__endpoint src, ip__endpoint dst, quic_server_test_stream::cid scid, quic_server_test_stream::cid dcid, quic_packet_type e, unsigned long long lo, unsigned long long hi);
    virtual void ext__infer_frame(quic_server_test_stream::cid scid, quic_server_test_stream::cid dcid, quic_server_test_stream::frame f);
    virtual void ext__tls__handshake_data_event(const ip__endpoint& src, const ip__endpoint& dst, const stream_data& data);
    virtual void imp__infer_frame(quic_server_test_stream::cid scid, quic_server_test_stream::cid dcid, quic_server_test_stream::frame f);
    virtual quic_transport_parameters ext__make_transport_parameters();
    virtual void ext__idle_timeout__set(const idle_timeout& p, trans_params_struct& s);
    virtual bool ext__quic_packet__long(const quic_packet& pkt);
    virtual void ext__frame__ack__handle(frame__ack f, quic_server_test_stream::cid scid, quic_server_test_stream::cid dcid, quic_packet_type e);
    virtual void ext__stateless_reset_token__set(const stateless_reset_token& p, trans_params_struct& s);
    virtual void ext__frame__arr__append(frame__arr& a, quic_server_test_stream::frame v);
    virtual quic_server_test_stream::cid ext__tls_id_to_cid(unsigned long long tls_id);
    virtual void ext__tls_api__lower__recv(unsigned long long c, const stream_data& data, int lev);
    virtual unsigned long long ext__vector__transport_parameter____domain__next(unsigned long long x);
    virtual unsigned long long ext__prot__get_var_int(const stream_data& pkt, unsigned long long pos, unsigned long long len);
    virtual unsigned long long ext__tls__handshakes__domain__next(unsigned long long x);
    virtual unsigned long long ext__frame__idx__next(unsigned long long x);
    virtual void ext__initial_max_stream_id_uni__set(const initial_max_stream_id_uni& p, trans_params_struct& s);
    virtual void ext__app_server_open_event(const ip__endpoint& src, const ip__endpoint& dst, quic_server_test_stream::cid scid, quic_server_test_stream::cid dcid);
    virtual unsigned long long ext__stream_max_data(quic_server_test_stream::cid dcid, unsigned id);
    virtual void ext__infer_tls_events(const ip__endpoint& src, const ip__endpoint& dst, const quic_packet& pkt);
    virtual unsigned ext__prot__byte_xor(unsigned x, unsigned y);
    virtual ip__endpoint ext__endpoint_id_addr(endpoint_id ep_id);
    virtual void ext__frame__stream_blocked__handle(const frame__stream_blocked& f, quic_server_test_stream::cid scid, quic_server_test_stream::cid dcid, quic_packet_type e);
    virtual prot__header_info ext__prot__get_header_info(const stream_data& pkt);
    virtual quic_packet ext__pkt_serdes__from_bytes(const stream_data& y);
    virtual void ext__undecryptable_packet_event(const ip__endpoint& src, const ip__endpoint& dst, const stream_data& pkt);
    virtual stream_data ext__tls_api__upper__encrypt_cipher(unsigned long long c, int l, const stream_data& clear, const stream_data& iv, bool recv);
    virtual void ext__stream_data__set(stream_data& a, unsigned long long x, unsigned y);
    virtual void tls_api__lower__send(unsigned long long c, const stream_data& data, int lev);
    virtual unsigned long long ext__prot__get_pnum_len(const stream_data& pkt, unsigned long long pnum_pos);
    virtual void ext__net__recv(endpoint_id me, int s, const ip__endpoint& src, const prot__arr& x);
    virtual void ext__prot__stream_data_xor(stream_data& x, const stream_data& y);
    virtual void ext__tls_keys_established_event(quic_server_test_stream::cid scid, quic_packet_type e);
    virtual void ext__packet_event(ip__endpoint src, ip__endpoint dst, quic_packet pkt);
    virtual void ext__initial_max_stream_data_bidi_remote__set(const initial_max_stream_data_bidi_remote& p, trans_params_struct& s);
    virtual void ext__frame__crypto__handle(frame__crypto f, quic_server_test_stream::cid scid, quic_server_test_stream::cid dcid, quic_packet_type e);
    virtual stream_data ext__cid_to_bytes(quic_server_test_stream::cid c, unsigned len);
    virtual void ext__tls_send_event(const ip__endpoint& src, const ip__endpoint& dst, quic_server_test_stream::cid scid, quic_server_test_stream::cid dcid, const stream_data& data, unsigned long long pos, quic_packet_type e);
    virtual void tls_api__upper__recv(unsigned long long c, const stream_data& data);
    virtual endpoint_id ext__endpoint_to_pid(const ip__endpoint& src);
    virtual void ext__stream_data__resize(stream_data& a, unsigned long long s, unsigned v);
    virtual tls_api__upper__decrypt_result ext__tls_api__upper__decrypt_aead(unsigned long long c, int l, const stream_data& cipher, unsigned seq, const stream_data& ad);
    virtual void ext__frame__application_close__handle(const frame__application_close& f, quic_server_test_stream::cid scid, quic_server_test_stream::cid dcid, quic_packet_type e);
    virtual void ext__max_ack_delay__set(const max_ack_delay& p, trans_params_struct& s);
    virtual quic_server_test_stream::cid ext__packet_scid(const prot__header_info& h);
    virtual unsigned long long ext__stream_pos__next(unsigned long long x);
    virtual void ext__initial_max_stream_data_uni__set(const initial_max_stream_data_uni& p, trans_params_struct& s);
    virtual void ext__app_send_event(const ip__endpoint& src, const ip__endpoint& dst, quic_server_test_stream::cid dcid, unsigned s, const stream_data& data, unsigned long long pos, bool close);
    virtual ip__endpoint ext__tls_id_to_src(unsigned long long tls_id);
    virtual int ext__endpoint_to_socket(const ip__endpoint& src);
    virtual void ext__handle_client_transport_parameters(const ip__endpoint& src, const ip__endpoint& dst, quic_server_test_stream::cid scid, const quic_transport_parameters& tps, bool is_client_hello);
    virtual void imp__tls__handshake_event(const ip__endpoint& src, const ip__endpoint& dst, quic_server_test_stream::tls__handshake h);
    virtual tls_api__upper__decrypt_result ext__prot__decrypt(unsigned long long c, unsigned seq, const stream_data& pkt);
    virtual void ext__frame__new_token__handle(const frame__new_token& f, quic_server_test_stream::cid scid, quic_server_test_stream::cid dcid, quic_packet_type e);
    virtual stream_data ext__pkt_serdes__to_bytes(const quic_packet& x);
    virtual void ext__set_encryption_level(const ip__endpoint& src, quic_server_test_stream::cid scid, quic_packet_type e);
    void __tick(int timeout);
};
inline bool operator ==(const quic_server_test_stream::tls__handshake &s, const quic_server_test_stream::tls__handshake &t);;
inline bool operator ==(const quic_server_test_stream::tls__extension &s, const quic_server_test_stream::tls__extension &t);;
inline bool operator ==(const quic_server_test_stream::frame &s, const quic_server_test_stream::frame &t);;
inline bool operator ==(const quic_server_test_stream::transport_parameter &s, const quic_server_test_stream::transport_parameter &t);;
inline bool operator ==(const quic_server_test_stream::tls__unknown_extension &s, const quic_server_test_stream::tls__unknown_extension &t){
    return ((s.etype == t.etype) && (s.content == t.content));
}
inline bool operator ==(const quic_server_test_stream::tls__random &s, const quic_server_test_stream::tls__random &t){
    return ((s.gmt_unix_time == t.gmt_unix_time) && (s.random_bytes == t.random_bytes));
}
inline bool operator ==(const quic_server_test_stream::tls__client_hello &s, const quic_server_test_stream::tls__client_hello &t){
    return ((s.client_version == t.client_version) && (s.rand_info == t.rand_info) && (s.session_id == t.session_id) && (s.cipher_suites == t.cipher_suites) && (s.compression_methods == t.compression_methods) && (s.extensions == t.extensions));
}
inline bool operator ==(const quic_server_test_stream::tls__server_hello &s, const quic_server_test_stream::tls__server_hello &t){
    return ((s.server_version == t.server_version) && (s.rand_info == t.rand_info) && (s.session_id == t.session_id) && (s.the_cipher_suite == t.the_cipher_suite) && (s.the_compression_method == t.the_compression_method) && (s.extensions == t.extensions));
}
inline bool operator ==(const quic_server_test_stream::tls__encrypted_extensions &s, const quic_server_test_stream::tls__encrypted_extensions &t){
    return ((s.extensions == t.extensions));
}
inline bool operator ==(const quic_server_test_stream::tls__unknown_message &s, const quic_server_test_stream::tls__unknown_message &t){
    return ((s.mtype == t.mtype) && (s.unknown_message_bytes == t.unknown_message_bytes));
}

bool operator ==(const quic_server_test_stream::tls__handshake &s, const quic_server_test_stream::tls__handshake &t){
    if (s.tag != t.tag) return false;
    switch (s.tag) {
        case 0: return quic_server_test_stream::tls__handshake::unwrap< quic_server_test_stream::tls__client_hello >(s) == quic_server_test_stream::tls__handshake::unwrap< quic_server_test_stream::tls__client_hello >(t);
        case 1: return quic_server_test_stream::tls__handshake::unwrap< quic_server_test_stream::tls__server_hello >(s) == quic_server_test_stream::tls__handshake::unwrap< quic_server_test_stream::tls__server_hello >(t);
        case 2: return quic_server_test_stream::tls__handshake::unwrap< quic_server_test_stream::tls__encrypted_extensions >(s) == quic_server_test_stream::tls__handshake::unwrap< quic_server_test_stream::tls__encrypted_extensions >(t);
        case 3: return quic_server_test_stream::tls__handshake::unwrap< quic_server_test_stream::tls__unknown_message >(s) == quic_server_test_stream::tls__handshake::unwrap< quic_server_test_stream::tls__unknown_message >(t);

    }
    return true;
}
inline bool operator ==(const quic_server_test_stream::ip__endpoint &s, const quic_server_test_stream::ip__endpoint &t){
    return ((s.protocol == t.protocol) && (s.addr == t.addr) && (s.port == t.port));
}
inline bool operator ==(const quic_server_test_stream::tls__handshake_parser__result &s, const quic_server_test_stream::tls__handshake_parser__result &t){
    return ((s.pos == t.pos) && (s.value == t.value));
}
inline bool operator ==(const quic_server_test_stream::frame__stream &s, const quic_server_test_stream::frame__stream &t){
    return ((s.off == t.off) && (s.len == t.len) && (s.fin == t.fin) && (s.id == t.id) && (s.offset == t.offset) && (s.length == t.length) && (s.data == t.data));
}
inline bool operator ==(const quic_server_test_stream::frame__ack__block &s, const quic_server_test_stream::frame__ack__block &t){
    return ((s.gap == t.gap) && (s.blocks == t.blocks));
}
inline bool operator ==(const quic_server_test_stream::frame__ack &s, const quic_server_test_stream::frame__ack &t){
    return ((s.largest_acked == t.largest_acked) && (s.ack_delay == t.ack_delay) && (s.ack_blocks == t.ack_blocks));
}
inline bool operator ==(const quic_server_test_stream::frame__rst_stream &s, const quic_server_test_stream::frame__rst_stream &t){
    return ((s.id == t.id) && (s.err_code == t.err_code) && (s.final_offset == t.final_offset));
}
inline bool operator ==(const quic_server_test_stream::frame__max_stream_id &s, const quic_server_test_stream::frame__max_stream_id &t){
    return ((s.id == t.id));
}
inline bool operator ==(const quic_server_test_stream::frame__connection_close &s, const quic_server_test_stream::frame__connection_close &t){
    return ((s.err_code == t.err_code) && (s.frame_type == t.frame_type) && (s.reason_phrase_length == t.reason_phrase_length) && (s.reason_phrase == t.reason_phrase));
}
inline bool operator ==(const quic_server_test_stream::frame__max_stream_data &s, const quic_server_test_stream::frame__max_stream_data &t){
    return ((s.id == t.id) && (s.pos == t.pos));
}
inline bool operator ==(const quic_server_test_stream::frame__stream_id_blocked &s, const quic_server_test_stream::frame__stream_id_blocked &t){
    return ((s.id == t.id));
}
inline bool operator ==(const quic_server_test_stream::frame__crypto &s, const quic_server_test_stream::frame__crypto &t){
    return ((s.offset == t.offset) && (s.length == t.length) && (s.data == t.data));
}
inline bool operator ==(const quic_server_test_stream::frame__ping &s, const quic_server_test_stream::frame__ping &t){
    return true;
}
inline bool operator ==(const quic_server_test_stream::frame__application_close &s, const quic_server_test_stream::frame__application_close &t){
    return ((s.err_code == t.err_code) && (s.reason_phrase_length == t.reason_phrase_length) && (s.reason_phrase == t.reason_phrase));
}
inline bool operator ==(const quic_server_test_stream::frame__new_connection_id &s, const quic_server_test_stream::frame__new_connection_id &t){
    return ((s.seq_num == t.seq_num) && (s.retire_prior_to == t.retire_prior_to) && (s.length == t.length) && (s.scid == t.scid) && (s.token == t.token));
}
inline bool operator ==(const quic_server_test_stream::frame__path_challenge &s, const quic_server_test_stream::frame__path_challenge &t){
    return ((s.data == t.data));
}
inline bool operator ==(const quic_server_test_stream::frame__path_response &s, const quic_server_test_stream::frame__path_response &t){
    return ((s.data == t.data));
}
inline bool operator ==(const quic_server_test_stream::frame__new_token &s, const quic_server_test_stream::frame__new_token &t){
    return ((s.length == t.length) && (s.data == t.data));
}
inline bool operator ==(const quic_server_test_stream::frame__max_data &s, const quic_server_test_stream::frame__max_data &t){
    return ((s.pos == t.pos));
}
inline bool operator ==(const quic_server_test_stream::frame__stream_blocked &s, const quic_server_test_stream::frame__stream_blocked &t){
    return ((s.id == t.id) && (s.pos == t.pos));
}
inline bool operator ==(const quic_server_test_stream::frame__stop_sending &s, const quic_server_test_stream::frame__stop_sending &t){
    return ((s.id == t.id) && (s.err_code == t.err_code));
}
inline bool operator ==(const quic_server_test_stream::frame__blocked &s, const quic_server_test_stream::frame__blocked &t){
    return ((s.pos == t.pos));
}
inline bool operator ==(const quic_server_test_stream::frame__retire_connection_id &s, const quic_server_test_stream::frame__retire_connection_id &t){
    return ((s.seq_num == t.seq_num));
}

bool operator ==(const quic_server_test_stream::frame &s, const quic_server_test_stream::frame &t){
    if (s.tag != t.tag) return false;
    switch (s.tag) {
        case 0: return quic_server_test_stream::frame::unwrap< quic_server_test_stream::frame__stream >(s) == quic_server_test_stream::frame::unwrap< quic_server_test_stream::frame__stream >(t);
        case 1: return quic_server_test_stream::frame::unwrap< quic_server_test_stream::frame__ack >(s) == quic_server_test_stream::frame::unwrap< quic_server_test_stream::frame__ack >(t);
        case 2: return quic_server_test_stream::frame::unwrap< quic_server_test_stream::frame__rst_stream >(s) == quic_server_test_stream::frame::unwrap< quic_server_test_stream::frame__rst_stream >(t);
        case 3: return quic_server_test_stream::frame::unwrap< quic_server_test_stream::frame__max_stream_id >(s) == quic_server_test_stream::frame::unwrap< quic_server_test_stream::frame__max_stream_id >(t);
        case 4: return quic_server_test_stream::frame::unwrap< quic_server_test_stream::frame__connection_close >(s) == quic_server_test_stream::frame::unwrap< quic_server_test_stream::frame__connection_close >(t);
        case 5: return quic_server_test_stream::frame::unwrap< quic_server_test_stream::frame__max_stream_data >(s) == quic_server_test_stream::frame::unwrap< quic_server_test_stream::frame__max_stream_data >(t);
        case 6: return quic_server_test_stream::frame::unwrap< quic_server_test_stream::frame__stream_id_blocked >(s) == quic_server_test_stream::frame::unwrap< quic_server_test_stream::frame__stream_id_blocked >(t);
        case 7: return quic_server_test_stream::frame::unwrap< quic_server_test_stream::frame__crypto >(s) == quic_server_test_stream::frame::unwrap< quic_server_test_stream::frame__crypto >(t);
        case 8: return quic_server_test_stream::frame::unwrap< quic_server_test_stream::frame__ping >(s) == quic_server_test_stream::frame::unwrap< quic_server_test_stream::frame__ping >(t);
        case 9: return quic_server_test_stream::frame::unwrap< quic_server_test_stream::frame__application_close >(s) == quic_server_test_stream::frame::unwrap< quic_server_test_stream::frame__application_close >(t);
        case 10: return quic_server_test_stream::frame::unwrap< quic_server_test_stream::frame__new_connection_id >(s) == quic_server_test_stream::frame::unwrap< quic_server_test_stream::frame__new_connection_id >(t);
        case 11: return quic_server_test_stream::frame::unwrap< quic_server_test_stream::frame__path_challenge >(s) == quic_server_test_stream::frame::unwrap< quic_server_test_stream::frame__path_challenge >(t);
        case 12: return quic_server_test_stream::frame::unwrap< quic_server_test_stream::frame__path_response >(s) == quic_server_test_stream::frame::unwrap< quic_server_test_stream::frame__path_response >(t);
        case 13: return quic_server_test_stream::frame::unwrap< quic_server_test_stream::frame__new_token >(s) == quic_server_test_stream::frame::unwrap< quic_server_test_stream::frame__new_token >(t);
        case 14: return quic_server_test_stream::frame::unwrap< quic_server_test_stream::frame__max_data >(s) == quic_server_test_stream::frame::unwrap< quic_server_test_stream::frame__max_data >(t);
        case 15: return quic_server_test_stream::frame::unwrap< quic_server_test_stream::frame__stream_blocked >(s) == quic_server_test_stream::frame::unwrap< quic_server_test_stream::frame__stream_blocked >(t);
        case 16: return quic_server_test_stream::frame::unwrap< quic_server_test_stream::frame__stop_sending >(s) == quic_server_test_stream::frame::unwrap< quic_server_test_stream::frame__stop_sending >(t);
        case 17: return quic_server_test_stream::frame::unwrap< quic_server_test_stream::frame__blocked >(s) == quic_server_test_stream::frame::unwrap< quic_server_test_stream::frame__blocked >(t);
        case 18: return quic_server_test_stream::frame::unwrap< quic_server_test_stream::frame__retire_connection_id >(s) == quic_server_test_stream::frame::unwrap< quic_server_test_stream::frame__retire_connection_id >(t);

    }
    return true;
}
inline bool operator ==(const quic_server_test_stream::initial_max_stream_data_bidi_local &s, const quic_server_test_stream::initial_max_stream_data_bidi_local &t){
    return ((s.stream_pos_32 == t.stream_pos_32));
}
inline bool operator ==(const quic_server_test_stream::initial_max_data &s, const quic_server_test_stream::initial_max_data &t){
    return ((s.stream_pos_32 == t.stream_pos_32));
}
inline bool operator ==(const quic_server_test_stream::initial_max_stream_id_bidi &s, const quic_server_test_stream::initial_max_stream_id_bidi &t){
    return ((s.stream_id_16 == t.stream_id_16));
}
inline bool operator ==(const quic_server_test_stream::idle_timeout &s, const quic_server_test_stream::idle_timeout &t){
    return ((s.seconds_16 == t.seconds_16));
}
inline bool operator ==(const quic_server_test_stream::preferred_address &s, const quic_server_test_stream::preferred_address &t){
    return true;
}
inline bool operator ==(const quic_server_test_stream::max_packet_size &s, const quic_server_test_stream::max_packet_size &t){
    return ((s.stream_pos_16 == t.stream_pos_16));
}
inline bool operator ==(const quic_server_test_stream::stateless_reset_token &s, const quic_server_test_stream::stateless_reset_token &t){
    return ((s.data_8 == t.data_8));
}
inline bool operator ==(const quic_server_test_stream::ack_delay_exponent &s, const quic_server_test_stream::ack_delay_exponent &t){
    return ((s.exponent_8 == t.exponent_8));
}
inline bool operator ==(const quic_server_test_stream::initial_max_stream_id_uni &s, const quic_server_test_stream::initial_max_stream_id_uni &t){
    return ((s.stream_id_16 == t.stream_id_16));
}
inline bool operator ==(const quic_server_test_stream::disable_migration &s, const quic_server_test_stream::disable_migration &t){
    return true;
}
inline bool operator ==(const quic_server_test_stream::initial_max_stream_data_bidi_remote &s, const quic_server_test_stream::initial_max_stream_data_bidi_remote &t){
    return ((s.stream_pos_32 == t.stream_pos_32));
}
inline bool operator ==(const quic_server_test_stream::initial_max_stream_data_uni &s, const quic_server_test_stream::initial_max_stream_data_uni &t){
    return ((s.stream_pos_32 == t.stream_pos_32));
}
inline bool operator ==(const quic_server_test_stream::max_ack_delay &s, const quic_server_test_stream::max_ack_delay &t){
    return ((s.exponent_8 == t.exponent_8));
}
inline bool operator ==(const quic_server_test_stream::active_connection_id_limit &s, const quic_server_test_stream::active_connection_id_limit &t){
    return ((s.stream_pos_32 == t.stream_pos_32));
}

bool operator ==(const quic_server_test_stream::transport_parameter &s, const quic_server_test_stream::transport_parameter &t){
    if (s.tag != t.tag) return false;
    switch (s.tag) {
        case 0: return quic_server_test_stream::transport_parameter::unwrap< quic_server_test_stream::initial_max_stream_data_bidi_local >(s) == quic_server_test_stream::transport_parameter::unwrap< quic_server_test_stream::initial_max_stream_data_bidi_local >(t);
        case 1: return quic_server_test_stream::transport_parameter::unwrap< quic_server_test_stream::initial_max_data >(s) == quic_server_test_stream::transport_parameter::unwrap< quic_server_test_stream::initial_max_data >(t);
        case 2: return quic_server_test_stream::transport_parameter::unwrap< quic_server_test_stream::initial_max_stream_id_bidi >(s) == quic_server_test_stream::transport_parameter::unwrap< quic_server_test_stream::initial_max_stream_id_bidi >(t);
        case 3: return quic_server_test_stream::transport_parameter::unwrap< quic_server_test_stream::idle_timeout >(s) == quic_server_test_stream::transport_parameter::unwrap< quic_server_test_stream::idle_timeout >(t);
        case 4: return quic_server_test_stream::transport_parameter::unwrap< quic_server_test_stream::preferred_address >(s) == quic_server_test_stream::transport_parameter::unwrap< quic_server_test_stream::preferred_address >(t);
        case 5: return quic_server_test_stream::transport_parameter::unwrap< quic_server_test_stream::max_packet_size >(s) == quic_server_test_stream::transport_parameter::unwrap< quic_server_test_stream::max_packet_size >(t);
        case 6: return quic_server_test_stream::transport_parameter::unwrap< quic_server_test_stream::stateless_reset_token >(s) == quic_server_test_stream::transport_parameter::unwrap< quic_server_test_stream::stateless_reset_token >(t);
        case 7: return quic_server_test_stream::transport_parameter::unwrap< quic_server_test_stream::ack_delay_exponent >(s) == quic_server_test_stream::transport_parameter::unwrap< quic_server_test_stream::ack_delay_exponent >(t);
        case 8: return quic_server_test_stream::transport_parameter::unwrap< quic_server_test_stream::initial_max_stream_id_uni >(s) == quic_server_test_stream::transport_parameter::unwrap< quic_server_test_stream::initial_max_stream_id_uni >(t);
        case 9: return quic_server_test_stream::transport_parameter::unwrap< quic_server_test_stream::disable_migration >(s) == quic_server_test_stream::transport_parameter::unwrap< quic_server_test_stream::disable_migration >(t);
        case 10: return quic_server_test_stream::transport_parameter::unwrap< quic_server_test_stream::initial_max_stream_data_bidi_remote >(s) == quic_server_test_stream::transport_parameter::unwrap< quic_server_test_stream::initial_max_stream_data_bidi_remote >(t);
        case 11: return quic_server_test_stream::transport_parameter::unwrap< quic_server_test_stream::initial_max_stream_data_uni >(s) == quic_server_test_stream::transport_parameter::unwrap< quic_server_test_stream::initial_max_stream_data_uni >(t);
        case 12: return quic_server_test_stream::transport_parameter::unwrap< quic_server_test_stream::max_ack_delay >(s) == quic_server_test_stream::transport_parameter::unwrap< quic_server_test_stream::max_ack_delay >(t);
        case 13: return quic_server_test_stream::transport_parameter::unwrap< quic_server_test_stream::active_connection_id_limit >(s) == quic_server_test_stream::transport_parameter::unwrap< quic_server_test_stream::active_connection_id_limit >(t);

    }
    return true;
}
inline bool operator ==(const quic_server_test_stream::trans_params_struct &s, const quic_server_test_stream::trans_params_struct &t){
    return ((s.initial_max_stream_data_bidi_local__is_set == t.initial_max_stream_data_bidi_local__is_set) && (s.initial_max_stream_data_bidi_local__value == t.initial_max_stream_data_bidi_local__value) && (s.initial_max_data__is_set == t.initial_max_data__is_set) && (s.initial_max_data__value == t.initial_max_data__value) && (s.initial_max_stream_id_bidi__is_set == t.initial_max_stream_id_bidi__is_set) && (s.initial_max_stream_id_bidi__value == t.initial_max_stream_id_bidi__value) && (s.idle_timeout__is_set == t.idle_timeout__is_set) && (s.idle_timeout__value == t.idle_timeout__value) && (s.preferred_address__is_set == t.preferred_address__is_set) && (s.preferred_address__value == t.preferred_address__value) && (s.max_packet_size__is_set == t.max_packet_size__is_set) && (s.max_packet_size__value == t.max_packet_size__value) && (s.stateless_reset_token__is_set == t.stateless_reset_token__is_set) && (s.stateless_reset_token__value == t.stateless_reset_token__value) && (s.ack_delay_exponent__is_set == t.ack_delay_exponent__is_set) && (s.ack_delay_exponent__value == t.ack_delay_exponent__value) && (s.initial_max_stream_id_uni__is_set == t.initial_max_stream_id_uni__is_set) && (s.initial_max_stream_id_uni__value == t.initial_max_stream_id_uni__value) && (s.disable_migration__is_set == t.disable_migration__is_set) && (s.disable_migration__value == t.disable_migration__value) && (s.initial_max_stream_data_bidi_remote__is_set == t.initial_max_stream_data_bidi_remote__is_set) && (s.initial_max_stream_data_bidi_remote__value == t.initial_max_stream_data_bidi_remote__value) && (s.initial_max_stream_data_uni__is_set == t.initial_max_stream_data_uni__is_set) && (s.initial_max_stream_data_uni__value == t.initial_max_stream_data_uni__value) && (s.max_ack_delay__is_set == t.max_ack_delay__is_set) && (s.max_ack_delay__value == t.max_ack_delay__value) && (s.active_connection_id_limit__is_set == t.active_connection_id_limit__is_set) && (s.active_connection_id_limit__value == t.active_connection_id_limit__value));
}
inline bool operator ==(const quic_server_test_stream::quic_transport_parameters &s, const quic_server_test_stream::quic_transport_parameters &t){
    return ((s.transport_parameters == t.transport_parameters));
}

bool operator ==(const quic_server_test_stream::tls__extension &s, const quic_server_test_stream::tls__extension &t){
    if (s.tag != t.tag) return false;
    switch (s.tag) {
        case 0: return quic_server_test_stream::tls__extension::unwrap< quic_server_test_stream::tls__unknown_extension >(s) == quic_server_test_stream::tls__extension::unwrap< quic_server_test_stream::tls__unknown_extension >(t);
        case 1: return quic_server_test_stream::tls__extension::unwrap< quic_server_test_stream::quic_transport_parameters >(s) == quic_server_test_stream::tls__extension::unwrap< quic_server_test_stream::quic_transport_parameters >(t);

    }
    return true;
}
inline bool operator ==(const quic_server_test_stream::quic_packet &s, const quic_server_test_stream::quic_packet &t){
    return ((s.ptype == t.ptype) && (s.pversion == t.pversion) && (s.dst_cid == t.dst_cid) && (s.src_cid == t.src_cid) && (s.token == t.token) && (s.seq_num == t.seq_num) && (s.payload == t.payload));
}
inline bool operator ==(const quic_server_test_stream::prot__header_info &s, const quic_server_test_stream::prot__header_info &t){
    return ((s.hdr_long == t.hdr_long) && (s.hdr_type == t.hdr_type) && (s.dcid == t.dcid) && (s.scid == t.scid) && (s.payload_length == t.payload_length) && (s.payload_length_pos == t.payload_length_pos) && (s.pkt_num_pos == t.pkt_num_pos));
}
inline bool operator ==(const quic_server_test_stream::tls_api__upper__decrypt_result &s, const quic_server_test_stream::tls_api__upper__decrypt_result &t){
    return ((s.ok == t.ok) && (s.data == t.data));
}
