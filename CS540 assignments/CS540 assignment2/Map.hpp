//
// Created by macbook on 2020/10/23.
//

#ifndef CS540_PROJ2_MAP_HPP
#define CS540_PROJ2_MAP_HPP

#include <cstring>
#include <algorithm>
#include <random>    /// generate random node height in insert()
#include <stdexcept> /// throw std::out_of_range in at()

namespace cs540{
    template <typename Key_T, typename Mapped_T>
    class Map { /// using skip list
        typedef std::pair<const Key_T, Mapped_T> ValueType;
    public:
        /// Nested classes
        class Iterator;
        class ConstIterator;
        class ReverseIterator;
        class SentinelNode;
        class DataNode;
        static const int MAX_LEVELS = 32; /// MAX_LEVELS doesn't mean skip list is fixed size;
//        static const int MAX_LEVELS = 4; /// MAX_LEVELS doesn't mean skip list is fixed size;
        SentinelNode *head; /// SentinelNode head
        SentinelNode *tail; /// SentinelNode tail
        size_t numNodes; /// number of nodes in Map(skip list)
        int height; /// height of skip list

        /// SentinelNode for head and tail, noo data inside the node.
        class SentinelNode {
        public:
            SentinelNode(){
                prev = nullptr;
                for (int i = 0; i < MAX_LEVELS; ++i)
                    nextNodes[i] = nullptr;
            }
            DataNode *nextNodes[MAX_LEVELS]; /// every node has MAX_LEVELS levels
            DataNode *prev; /// prev node
        }; //end class SentinelNode

        /// DataNode : extension from SentinelNode for head and tail, noo data inside the node.
        class DataNode : public SentinelNode {
        public:
            DataNode() = delete;
            DataNode(ValueType valueIn) : value(valueIn) {}
            ValueType value;
        }; //end class DataNode

        /// ========================================================
        ///         Part 1 : Constructors and Assignment Operator
        /// ========================================================

        /// (1) This constructor creates an empty map.
        Map() {
            numNodes = 0; /// empty map has no nodes
            height = 0; /// height == 0
            /// new SentinelNode for head and tail
            head = new SentinelNode;
            tail = new SentinelNode;
            for (int curLevel = 0; curLevel < MAX_LEVELS; ++curLevel)
                head->nextNodes[curLevel] = static_cast<DataNode*>(tail);
            tail->prev = static_cast<DataNode*>(head);
        }

        /// (2) Copy constructor. Must have O(N) performance, where N is the number of elements.
        Map(const Map &map) {
            numNodes = 0;
            height = 0;
            head = new SentinelNode;
            tail = new SentinelNode;
            for (int curLevel = 0; curLevel < MAX_LEVELS; ++curLevel)
                head->nextNodes[curLevel] = static_cast<DataNode*>(tail);
            tail->prev = static_cast<DataNode*>(head);

            DataNode *trav = map.head->nextNodes[0];
            while (trav != static_cast<DataNode*>(map.tail)) {
                insert(trav->value);
                trav = trav->nextNodes[0];
            }
        }

        /// (3) Copy assignment operator. Value semantics must be used.
        /// You must be able to handle self-assignment.
        /// Must have O(N) performance, where N is the number of elements.
        Map& operator= (const Map &map) {
            if (&map != this) {  /// check for (and ignore) self assignment
                clear();
                DataNode *trav = map.head->nextNodes[0]; /// get the first node in the list

                for (int curLevel = 0; curLevel < MAX_LEVELS; ++curLevel)
                    head->nextNodes[curLevel] = static_cast<DataNode*>(tail);
                tail->prev = static_cast<DataNode*>(head);

                while (trav != static_cast<DataNode*>(map.tail)) {
                    insert(trav->value);
                    trav = trav->nextNodes[0];
                }
            }
            return *this; /// when this == &map, means: self assignment, return *this directly
        }

        /// (4) Initializer list constructor. Support for creation of Map with initial values,
        /// such as :
        ///     Map<string,int> m{{"key1", 1}, {"key2", 2}};
        Map(std::initializer_list<std::pair<const Key_T, Mapped_T>> initList) {
            numNodes = 0;
            height = 0;
            head = new SentinelNode;
            tail = new SentinelNode;
            for (int curLevel = 0; curLevel < MAX_LEVELS; ++curLevel){
                head->nextNodes[curLevel] = static_cast<DataNode*>(tail);
            }
            tail->prev = static_cast<DataNode*>(head);
            for (auto it = initList.begin(); it != initList.end(); ++it){
                insert ({it->first, it->second});
            }
        }

        /// (5) Destructor, release any acquired resources.
        ~Map() {
            /// clear and delete head and tail
            clear();
            delete head;
            delete tail;
        }

        /// ========================================================
        ///                     Part 2 : Size
        /// ========================================================

        /// (1) Returns the number of elements in the map.
        size_t size() const { return numNodes; }
        /// (2) Returns true if the Map has no entries in it, false otherwise.
        bool empty() const { return (numNodes == 0);}

        /// ========================================================
        ///                    Part 3 : Iterators
        /// ========================================================

        /// (1) Returns an Iterator pointing to the first element, in order.
        Iterator begin() {
            Iterator iterator (head->nextNodes[0]);
            return iterator;
        }

        /// (2) Returns an Iterator pointing one past the last element(is tail), in order.
        Iterator end() {
            Iterator iterator(static_cast<DataNode*>(tail));
            return iterator;
        }

        /// (3) Returns a ConstIterator pointing to the first element, in order.
        ConstIterator begin() const {
            ConstIterator iterator (head->nextNodes[0]);
            return iterator;
        }

        /// (4) Returns a ConstIterator pointing one past the last element(tail), in order.
        ConstIterator end() const {
            ConstIterator iterator(static_cast<DataNode*>(tail));
            return iterator;
        }

        /// (5) Returns an ReverseIterator pointing to the first element in reverse order,
        /// which is the last element in normal order.
        ReverseIterator rbegin() { /// Means the last elements
            ReverseIterator iterator(tail->prev);
            return iterator;
        }
        /// (6) Returns an ReverseIterator pointing to one past the last element in reverse order,
        ///  which is one before the first element in normal order.
        ReverseIterator rend() { /// Means the head
            ReverseIterator iterator(static_cast<DataNode*>(head));
            return iterator;
        }

        /// ========================================================
        ///                    Part 4 : Element Access
        /// ========================================================

        /// (1) Returns an iterator to the given key.
        Iterator find(const Key_T &key) {
            /// so we search from the highest level: height start from 0, so the highest number is height-1;
            int curLevel = height - 1;
            /// use cur to traverse the skip list to find the key
            auto cur = static_cast<DataNode*>(head);
            while (curLevel >= 0) {
                /// .first is Key_T key; bc Map is sorted by key, so we just need to compare key.
                /// (1) First to do search horizontally
                while (cur->nextNodes[curLevel] != static_cast<DataNode*>(tail)
                && cur->nextNodes[curLevel]->value.first < key){
                    cur = cur->nextNodes[curLevel];
                }
                /// Check if cur is the element we are finding:
                if (cur->nextNodes[curLevel] != static_cast<DataNode*>(tail)
                && cur->nextNodes[curLevel]->value.first == key){
                    return cur->nextNodes[curLevel];
                }
                ///  (2) if not find in this level, then search vertically
                --curLevel;
            }
            /// key < 0 means: key is not found, return the end() iterator.
            return static_cast<DataNode*>(tail);
        }

        /// (2) Returns an iterator to the given key. Just use Iterator find() function
        ConstIterator find(const Key_T & key) const {
            return find(key);
        }

        /// (3) Returns a reference to the mapped object at the specified key.
        Mapped_T &at(const Key_T &key) {
            auto it = find(key);
            /// If the key is not in the Map, throws std::out_of_range.
            if (it == end()){
                throw std::out_of_range("value not found while using at()");
            }
            return (*it).second; /// return mapped obj
        }

        /// (4) Returns a const reference to the mapped object at the specified key.
        /// If the key is not in the map, throws std::out_of_range.
        const Mapped_T &at(const Key_T &key) const {
            return at(key);
        }

        /// (5) If key is in the map, return a reference to the corresponding mapped object.
        /// If it is not, value initialize a mapped object for that key and returns a reference to it (after insert).
        ///     This operator may not be used for a Mapped_T class type that does not support default construction.
        Mapped_T &operator[](const Key_T &key) { ///
            /// So first we need to try to find the key in the map
            auto retIt = find(key);
            /// If it is not:
            if (retIt == end()) {
                /// (1) value initialize a mapped object for that key
                Mapped_T map;
                ValueType val(key, map);
                /// (2) returns a reference to it (after insert).
                ///     The return value of insert is <Iterator, bool>, and the return value should be val
                /// so retPair.first.second means value;
                auto retPair = insert(val);
                return (*retPair.first).second;
            }
            /// If key is in the map, return a reference to the corresponding mapped object.
            return (*retIt).second;
        }

        /// ========================================================
        ///                     Part 5 : Modifiers
        /// ========================================================


        /// (1) The first version inserts the given pair into the map.

        std::pair<Iterator, bool> insert(const ValueType &value) { /// !!!
            /// (i) If the key already exists, no insertion is performed nor is the mapped object changed;
            auto retIt = find(value.first);
            if (retIt != end()) {
                ///  And it returns an iterator pointing to the element with the same key, and false.
                std::pair<Iterator, bool> retPair = {retIt, false};
                return retPair;
            }
            /// (ii) If the key does not already exist in the map,
            /// it returns an iterator pointing to the new element, and true.
            /// (A) Random choose insert height:
            /// random give repeat value 0 or 1 to decide if increase the height;
            std::uniform_int_distribution<int> distribution(0,1);
            std::default_random_engine e; //// !!!!!
            int insertHeight = 1;
            bool repeat = true;
            /// we use MAX_LEVELS to make it not too high.
            while (repeat && insertHeight < MAX_LEVELS) {
                repeat = distribution(e); /// std::default_random_engine e;
                insertHeight++;
            }
            if (insertHeight > height) height = insertHeight;
            /// new newNode for inserting
            auto newNode = new DataNode(value);
            /// update num of nodes in Map
            ++numNodes;
            int curLevel = height - 1;
            auto trav = static_cast<DataNode*>(head);
            DataNode *tmp;
            while (curLevel >= 0) {
                while (trav->nextNodes[curLevel] != static_cast<DataNode*>(tail)
                && trav->nextNodes[curLevel]->value.first < value.first){
                    trav = trav->nextNodes[curLevel];
                }
                if (curLevel < insertHeight) {
                    tmp = trav->nextNodes[curLevel];
                    trav->nextNodes[curLevel] = newNode;
                    newNode->nextNodes[curLevel] = tmp;
                    if (curLevel == 0) {
                        tmp->prev = newNode;
                        newNode->prev = trav;
                    }
                }
                --curLevel;
            }
            return {{newNode}, true};
        }

        /// （2) The second version inserts the given object or range of objects into the map.
        ///     In the second version, the range of objects inserted includes the object
        /// range_beg points to, but not the object that range_end points to.
        ///     In other words, the range is half-open.
        ///     The iterator returned in the first version points to the newly inserted
        /// element. There must be only one constructor invocation per object inserted.
        ///     Note that the range may be in a different container type, as long as
        /// the iterator is compatible.
        ///     A compatible iterator would be one from which a ValueType can be constructed.
        ///     For example, it might be from a std::vector<std::pair<Key_T, Mapped_T>>.
        ///     There might be any number of compatible iterator types, therefore,
        /// the range insert is a member template.

        /// In the other words, this is used for things like vector, insert from begin to end;
        template<typename IT_T>
        void insert(IT_T range_beg, IT_T range_end) { /// !!!
            for (IT_T it = range_beg; it != range_end; it++) {
                insert({(*it).first, (*it).second});
            }
        }

        /// (3) Removes the given object from the map.
        /// The object may be indicated by iterator, or by key.
        /// If given by key, throws std::out_of_range if the key is not in the Map
        void erase(Iterator pos) {
            erase((*pos).first);
        }

        /// (4) Removes the given object from the map.
        /// The object may be indicated by iterator, or by key.
        /// If given by key, throws std::out_of_range if the key is not in the Map
        void erase(const Key_T &key) { /// !!!
            DataNode *toDelete = nullptr;
            int curLevel = height - 1;
            auto trav = static_cast<DataNode*>(head);
            while (curLevel >= 0) {
                while (trav->nextNodes[curLevel] != static_cast<DataNode*>(tail)
                &&  trav->nextNodes[curLevel]->value.first < key){
                    trav = trav->nextNodes[curLevel];
                }
                if (trav->nextNodes[curLevel] != static_cast<DataNode*>(tail)
                && trav->nextNodes[curLevel]->value.first == key) {
                    toDelete = trav->nextNodes[curLevel]; /// we find the node which need to be deleted
                    trav->nextNodes[curLevel] = trav->nextNodes[curLevel]->nextNodes[curLevel];
                    if (curLevel == 0){ trav->nextNodes[curLevel]->prev = trav; }
                }
                --curLevel;
            }
            /// if we cannot find the node after searching the map:
            if (toDelete == nullptr) throw std::out_of_range("attempted to delete a key which is not in the map");
            /// else : do delete
            delete toDelete;
            --numNodes;
            /// update height in case we just deleted the only elem from the top level
            /// Balance the Skip list with new height
            for (curLevel = height - 1; curLevel >= 0; --curLevel) {
                if (head->nextNodes[curLevel] != static_cast<DataNode*>(tail)) {
                    height = curLevel + 1;
                    break;
                }
            }
        }

        /// (5) Removes all elements from the map.
        void clear() {
            ///     Map is build by nodes, so we just use one node traverse from the start
            /// and delete every node in the map;
            DataNode *trav = head->nextNodes[0];
            DataNode *prev;
            while (trav != static_cast<DataNode*>(tail)) {
                prev = trav;
                trav = trav->nextNodes[0];
                delete prev;
            }
            for (int curLevel = 0; curLevel < MAX_LEVELS; ++curLevel){
                head->nextNodes[curLevel] = static_cast<DataNode*>(tail);
            }
            tail->prev = static_cast<DataNode*>(head);
            height = 0;
            numNodes = 0;
        }


        /// ========================================================
        ///                     Part 6 : Comparison
        /// ========================================================

        ///     These operators may be implemented as member functions or free functions,
        /// though implementing as free functions is recommended.

        /// (1) The first operator compares the given maps for equality.
        ///     Two maps compare equal :

        friend bool operator==(const Map &mp1, const Map &mp2) {
            /// (i) if they have the same number of elements,
            if (mp1.numNodes != mp2.numNodes) return false;
            /// (ii) if all elements compare equal.
            auto it1 = mp1.begin(), it2 = mp2.begin();
            while (it1 != mp1.end()) { /// compare all the element in Map
                if (!(it1->first == it2->first)
                || !(it1->second == it2->second)){
                    return false;
                }
                it1++; it2++;
            }
            return true;
        }

        /// (2) The second operator compares the given maps for inequality.
        /// You may implement this simply as the logical complement of the equality operator.
        friend bool operator!= (const Map &mp1, const Map &mp2) {
            return (!(mp1 == mp2));
        }

        /// (3) The third operator, you must use lexicographic sorting.
        /// Corresponding elements from each maps must be compared one-by-one.
        ///     Map elements are of type ValueType, so this actually compares the pairs.
        friend bool operator<  (const Map &lhs, const Map &rhs) {
            ///     A map M1 is less than a map M2 has two conditions：
            /// (i) if there is an element in M1 that is less than
            /// the corresponding element in the same position in map M2,
            auto lhsIt = lhs.begin();
            auto rhsIt = rhs.begin();
            while (lhsIt != lhs.end() && rhsIt != rhs.end()) {
                if ((*lhsIt) < (*rhsIt)) return true;
                lhsIt++; rhsIt++;
            }
            /// (ii) or if all corresponding elements in both maps are equal and M1
            /// is shorter than M2.
            return lhs.numNodes < rhs.numNodes;
        }

        /// ========================================================
        ///          Part 7 : Map<Key_T, Mapped_T>::ConstIterator
        /// ========================================================

        /// This class has all the same functions and operators as the Iterator class,
        /// except that the dereference operator (*) and the class member access operator (->),
        /// better known as the arrow operator, return const references.
        ///     You should try to move as many of the operations below as possible into
        /// a base class that is common to the other iterator types.

        class ConstIterator {
//            friend bool operator== (const Iterator &, const ConstIterator &);
//            friend bool operator== (const ConstIterator &, const Iterator &);
//            friend bool operator!= (const Iterator &, const ConstIterator &);
//            friend bool operator!= (const ConstIterator &, const Iterator &);
        public:
            DataNode *cur;

            ConstIterator (DataNode *datanode){
                cur = datanode;
            }
            /// Your class must have a copy constructor, but you do not need to define
            /// this if the implicit one works for your implementation.
            /// (Which is what I expect in most cases.)
            /// This is a conversion operator.
            explicit ConstIterator (const Iterator &iterator) : cur (iterator.cur) {}

            /// Destructor (implicit definition is likely good enough).
            ~ConstIterator() = default;
            /// Your class must have an assignment operator, but you do not need to define this
            /// if the implicit one works for your implementation.
            /// (Which is what I expect in most cases.)
//            ConstIterator &operator=(const ConstIterator &) {}
            /// Increments the iterator one element, and returns a reference to the
            /// incremented iterator (preincrement).
            /// If the iterator is pointing to the end of the list, the behavior is undefined.

            ConstIterator &operator++() {
                cur = cur->nextNodes[0];
                return *this;
            }
            /// Increments the iterator one element, and returns an iterator pointing to the
            /// element prior to incrementing the iterator (postincrement).
            /// If the iterator is pointing to the end of the list, the behavior is undefined.
            ConstIterator operator++(int) {
                auto tmp = cur;
                cur = cur->nextNodes[0];
                return tmp;
            }
            /// Decrements the iterator one element, and returns a reference to the decremented
            /// iterator (predecrement).
            /// If the iterator is pointing to the beginning of the list, the behavior is undefined.
            /// if the iterator has the special value returned by the end() function,
            /// then the iterator must point to the last element after this function.
            ConstIterator &operator--() {
                cur = cur->prev;
                return *this;
            }
            /// Decrements the iterator one element, and returns an iterator pointing to the element
            /// prior to decrementing (postdecrement).
            /// If the iterator is pointing to the beginning of the list, the behavior is undefined.
            /// if the iterator has the special value returned by the end() function,
            /// then the iterator must point to the last element after this function.
            ConstIterator operator--(int) {
                auto tmp = cur;
                cur = cur->prev;
                return tmp;
            }

            /// Returns a reference to the current element of the iterator.
            /// If the iterator is pointing to the end of the list, the behavior is undefined.
            const ValueType& operator*() const {
                return cur->value;
            }
            /// Special member access operator for the element.
            /// If the iterator is pointing to the end of the list, the behavior is undefined.
            const ValueType *operator->() const {
                return &cur->value;
            }

        };//end class ConstIterator

        /// ========================================================
        ///          Part 8 : Map<Key_T, Mapped_T>::Iterator
        /// ========================================================
        class Iterator {
            friend ConstIterator::ConstIterator(const Iterator &);
        public:
            DataNode *cur;
            Iterator(DataNode *node){
                cur = node;
            }
            /// (1) Your class must have a copy constructor, but you do not need to define
            /// this if the implicit one works for your implementation.
            /// (Which is what I expect in most cases.)
//            Iterator(const Iterator &iterator) {}

            /// (2) Destructor (implicit definition is likely good enough).
            ~Iterator() = default;

            /// (3) Your class must have an assignment operator, but you do not need to
            /// define this if the implicit one works for your implementation.
            /// (Which is what I expect in most cases.)
//            Iterator &operator=(const Iterator &iterator) {}


            /// (4) Increments the iterator one element, and returns a reference to the
            /// incremented iterator (preincrement).
            ///     If the iterator is pointing to the end of the list,
            /// the behavior is undefined.
            Iterator& operator++() {
                cur = cur->nextNodes[0];
                return *this;
            }

            /// (5) Increments the iterator one element, and returns an iterator pointing
            /// to the element prior to incrementing the iterator (postincrement).
            ///     If the iterator is pointing to the end of the list,the behavior is undefined.
            Iterator operator++(int) {
                auto tmp = cur; /// recorded cur and then do ++
                cur = cur->nextNodes[0];
                return tmp; /// return cur before ++
            }

            /// (6) Decrements the iterator one element, and returns a reference to the
            /// decremented iterator (predecrement).
            ///     If the iterator is pointing to the beginning of the list,
            /// the behavior is undefined.
            ///     If the iterator has the special value returned by the end() function,
            /// then the iterator must point to the last element after this function.
            Iterator &operator--() {
                cur = cur->prev;
                return *this;
            }

            /// (7) Decrements the iterator one element, and returns an iterator pointing
            /// to the element prior to decrementing (postdecrement).
            Iterator operator--(int) {
                auto tmp = cur;
                cur = cur->prev;
                return tmp;
            }

            /// (8) Returns a reference to the ValueType object contained in this element
            /// of the list.
            ///     If the iterator is pointing to the end of the list, the behavior is
            /// undefined.
            ///     This can be used to change the Mapped_T member of the element.
            ValueType &operator*() const {
                return cur->value;
            }

            /// (9) Special member access operator for the element.
            ///     If the iterator is pointing to the end of the list, the behavior is
            /// undefined.
            ///     This can be used to change the Mapped_T member of the element.
            ValueType *operator->() const {
                return &cur->value;
            }

        };//end class Iterator

        /// ========================================================
        ///          Part 9 : Map<Key_T, Mapped_T>::ReverseIterator
        /// ========================================================

        ///     This class has all the same functions and operators as the Iterator class,
        /// except that the direction of increment and decrement are reversed.
        ///     In other words, incrementing this iterator actually goes backwards through the map.
        ///     You should try to move as many of the operations below as possible into a base
        /// class that is common to the other iterator types.
        ///     Note that a real container would probably also have a const reverse iterator,
        /// which would result in even more duplication.
        class ReverseIterator {
        public:
            DataNode *cur;
            ReverseIterator(DataNode *node) : cur(node) {}
            /// Destructor (implicit definition is likely good enough).
            ~ReverseIterator()= default;

            /// Increments the iterator one element, and returns a reference to the incremented
            /// iterator (preincrement).
            /// If the iterator is pointing to the end of the list, the behavior is undefined.
            ReverseIterator &operator++() {
                cur = cur->prev;
                return *this;
            }
            /// Increments the iterator one element, and returns an iterator pointing to the
            /// element prior to incrementing the iterator (postincrement).
            /// If the iterator is pointing to the end of the list, the behavior is undefined.
            ReverseIterator operator++(int) {
                DataNode* tmp = cur;
                cur = cur->prev; /// opposite to the constIt and It, move back cur in ++();
                return tmp;
            }
            /// Decrements the iterator one element, and returns a reference to the decremented
            /// iterator (predecrement).
            ///     If the iterator is pointing to the beginning of the list,
            /// the behavior is undefined.
            /// If the iterator has the special value returned by the end() function,
            /// then the iterator must point to the last element after this function.
            ReverseIterator &operator--() {
                cur = cur->nextNodes[0];
                return *this;
            }

            /// Decrements the iterator one element, and returns an iterator pointing to the
            /// element prior to decrementing (postdecrement).
            ReverseIterator operator--(int) {
                DataNode* tmp = cur;
                cur = cur->nextNodes[0]; /// opposite to the constIt and It, move forward cur in ++();
                return tmp;
            }

            /// Returns a reference to the ValueType object contained in this element of the list.
            /// If the iterator is pointing to the end of the list, the behavior is undefined.
            /// This can be used to change the Mapped_T member of the element.
            ValueType &operator*() const {
                return cur->value;
            }
            /// Special member access operator for the element. If the iterator is pointing to
            /// the end of the list, the behavior is undefined.
            ///     This can be used to change the Mapped_T member of the element.
            ValueType *operator->() const {
                return &cur->value;
            }
        };

        /// ========================================================
        ///          Part 10 : Comparison Operators for Iterators
        /// ========================================================

        ///     These operators implemented as member functions or free functions.
        ///     I suggest that you use free functions, however.
        ///     You must be able to compare any combination of Iterator and ConstIterator.
        ///     Two iterators compare equal if they point to the same element in the list.
        ///     Two iterators may compare unequal even if the T objects that they contain
        /// compare equal.
        ///     It's not strictly necessary that you implement the above exactly as written,
        /// only that you must be able to compare the above.
        ///     For example, if your Iterator inherits from ConstIterator, then you may be
        /// able to get some of the above comparisons autumatically via implicit upcasts.

        /// ========================================================
        friend bool operator==(const ConstIterator & lhs, const ConstIterator &rhs){
            return (lhs.cur == rhs.cur);
        }
        friend bool operator!=(const ConstIterator &lhs, const ConstIterator &rhs){
            return (lhs.cur != rhs.cur);
        }
        /// ========================================================
        friend bool operator==(const Iterator &lhs, const Iterator &rhs){
            return (lhs.cur == rhs.cur);
        }
        friend bool operator!=(const Iterator &lhs, const Iterator &rhs){
            return (lhs.cur != rhs.cur);
        }
        /// =======================================================
        friend bool operator==(const ConstIterator &lhs, const Iterator &rhs){
            return (lhs.cur == rhs.cur);
        }
        friend bool operator==(const Iterator &lhs, const ConstIterator &rhs){
            return (lhs.cur == rhs.cur);
        }
        friend bool operator!=(const ConstIterator &lhs, const Iterator &rhs){
            return (lhs.cur != rhs.cur);
        }
        friend bool operator!=(const Iterator &lhs, const ConstIterator &rhs){
            return (lhs.cur != rhs.cur);
        }
        /// ========================================================
        friend bool operator==(const ReverseIterator &lhs, const ReverseIterator &rhs){
            return (lhs.cur == rhs.cur);
        }
        friend bool operator!=(const ReverseIterator &lhs, const ReverseIterator &rhs){
            return (lhs.cur != rhs.cur);
        }
        /// ========================================================
    };
}

#endif //CS540_PROJ2_MAP_HPP
