



#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <limits>
#include <cstring>
#include <utility>
#include <climits>
using namespace std;

static const int D = 2;                  
static const int MIN_KEYS = D;           
static const int MAX_KEYS = 2 * D;       
static const int NAME_SIZE = 40;
static const int PHONE_SIZE = 20;

struct Cliente {
    int codCliente = 0;
    char nome[NAME_SIZE] = {0};
    int idade = 0;
    char telefone[PHONE_SIZE] = {0};
};

struct MetaData {
    int order = D;
    long long rootOffset = -1;      
    long long nextNodeId = 1;       
    long long nextRecordId = 1;     
};

struct IndexNode {
    
    long long id = 0;
    bool isLeaf = true;
    int keyCount = 0;
    int keys[MAX_KEYS + 1] = {0};               
    long long children[MAX_KEYS + 2] = {0};     
    long long records[MAX_KEYS + 1] = {0};      
    long long nextLeaf = -1;                    

    
    long long selfOffset = -1;
};

class FileManager {
public:
    string metaPath, indexPath, dataPath;
    MetaData meta{};
    fstream metaFile, indexFile, dataFile;

    FileManager(const string& baseDir = ".",
                const string& metaName = "metadata.bin",
                const string& indexName = "index.bin",
                const string& dataName = "data.bin") {
        metaPath = baseDir + "/" + metaName;
        indexPath = baseDir + "/" + indexName;
        dataPath = baseDir + "/" + dataName;

        
        openCreate(metaFile, metaPath);
        openCreate(indexFile, indexPath);
        openCreate(dataFile, dataPath);

        
        if (fileSize(metaFile) >= (streamsize)sizeof(MetaData)) {
            metaFile.seekg(0);
            metaFile.read(reinterpret_cast<char*>(&meta), sizeof(MetaData));
        } else {
            persistMeta();
        }
    }

    ~FileManager() {
        if (metaFile.is_open()) persistMeta();
        metaFile.close();
        indexFile.close();
        dataFile.close();
    }

    void persistMeta() {
        metaFile.seekp(0);
        metaFile.write(reinterpret_cast<const char*>(&meta), sizeof(MetaData));
        metaFile.flush();
    }

    long long appendIndexNode(IndexNode& node) {
        indexFile.seekp(0, ios::end);
        long long offset = indexFile.tellp();
        node.selfOffset = offset;
        indexFile.write(reinterpret_cast<const char*>(&node), sizeof(IndexNode));
        indexFile.flush();
        return offset;
    }

    void writeIndexNode(const IndexNode& node) {
        indexFile.seekp(node.selfOffset);
        indexFile.write(reinterpret_cast<const char*>(&node), sizeof(IndexNode));
        indexFile.flush();
    }

    bool readIndexNode(long long offset, IndexNode& node) {
        if (offset < 0) return false;
        indexFile.seekg(offset);
        if (!indexFile.read(reinterpret_cast<char*>(&node), sizeof(IndexNode))) return false;
        return true;
    }

    long long appendRecord(const Cliente& c) {
        dataFile.seekp(0, ios::end);
        long long offset = dataFile.tellp();
        dataFile.write(reinterpret_cast<const char*>(&c), sizeof(Cliente));
        dataFile.flush();
        return offset;
    }

    bool readRecord(long long offset, Cliente& c) {
        if (offset < 0) return false;
        dataFile.seekg(offset);
        return (bool)dataFile.read(reinterpret_cast<char*>(&c), sizeof(Cliente));
    }

    void writeRecord(long long offset, const Cliente& c) {
        dataFile.seekp(offset);
        dataFile.write(reinterpret_cast<const char*>(&c), sizeof(Cliente));
        dataFile.flush();
    }

private:
    static void openCreate(fstream& f, const string& path) {
        f.open(path, ios::in | ios::out | ios::binary);
        if (!f.is_open()) {
            f.clear();
            f.open(path, ios::out | ios::binary);
            f.close();
            f.open(path, ios::in | ios::out | ios::binary);
        }
    }

    static streamsize fileSize(fstream& f) {
        auto pos = f.tellg();
        f.seekg(0, ios::end);
        auto size = f.tellg();
        f.seekg(pos);
        return size;
    }
};

class BPlusTree {
public:
    FileManager fm;

    BPlusTree(const string& baseDir = ".") : fm(baseDir) {
        if (fm.meta.rootOffset < 0) {
            
            IndexNode root;
            root.id = fm.meta.nextNodeId++;
            root.isLeaf = true;
            root.keyCount = 0;
            root.nextLeaf = -1;
            long long off = fm.appendIndexNode(root);
            fm.meta.rootOffset = off;
            fm.persistMeta();
        }
    }

    bool search(int key, Cliente& out) {
        IndexNode leaf;
        if (!descendToLeaf(key, leaf)) return false;
        int pos = lowerBound(leaf, key);
        if (pos < leaf.keyCount && leaf.keys[pos] == key) {
            return fm.readRecord(leaf.records[pos], out);
        }
        return false;
    }

    bool insert(const Cliente& c) {
        IndexNode leaf;
        if (!descendToLeaf(c.codCliente, leaf)) return false;

        int pos = lowerBound(leaf, c.codCliente);
        if (pos < leaf.keyCount && leaf.keys[pos] == c.codCliente) {
            
            fm.writeRecord(leaf.records[pos], c);
            return true;
        }

        
        if (leaf.keyCount < MAX_KEYS) {
            shiftRightLeaf(leaf, pos);
            leaf.keys[pos] = c.codCliente;
            leaf.records[pos] = fm.appendRecord(c);
            leaf.keyCount++;
            fm.writeIndexNode(leaf);
            return true;
        } else {
            
            splitLeafAndInsert(leaf, c);
            return true;
        }
    }

    bool erase(int key) {
        vector<pair<long long, int>> path; 
        IndexNode leaf;
        if (!descendToLeafWithPath(key, leaf, path)) return false;

        int pos = lowerBound(leaf, key);
        if (pos >= leaf.keyCount || leaf.keys[pos] != key) return false;

        
        shiftLeftLeaf(leaf, pos);
        leaf.keyCount--;
        fm.writeIndexNode(leaf);

        
        fixDeletion(leaf, path);
        return true;
    }

    
    void printAll() {
        
        IndexNode root;
        fm.readIndexNode(fm.meta.rootOffset, root);

        
        IndexNode cur = root;
        while (!cur.isLeaf) {
            IndexNode child;
            fm.readIndexNode(cur.children[0], child);
            cur = child;
        }

        while (true) {
            cout << "[Leaf id=" << cur.id << " count=" << cur.keyCount << "] ";
            for (int i = 0; i < cur.keyCount; ++i) cout << cur.keys[i] << " ";
            cout << "\n";
            if (cur.nextLeaf < 0) break;
            fm.readIndexNode(cur.nextLeaf, cur);
        }
    }

private:
    
    bool descendToLeaf(int key, IndexNode& leaf) {
        IndexNode node;
        if (!fm.readIndexNode(fm.meta.rootOffset, node)) return false;
        while (!node.isLeaf) {
            int i = upperBound(node, key);
            long long childOff = node.children[i];
            fm.readIndexNode(childOff, node);
        }
        leaf = node;
        return true;
    }

    bool descendToLeafWithPath(int key, IndexNode& leaf, vector<pair<long long, int>>& path) {
        IndexNode node;
        if (!fm.readIndexNode(fm.meta.rootOffset, node)) return false;
        while (!node.isLeaf) {
            int i = upperBound(node, key);
            path.push_back({node.selfOffset, i});
            long long childOff = node.children[i];
            fm.readIndexNode(childOff, node);
        }
        leaf = node;
        return true;
    }

    
    static int lowerBound(const IndexNode& node, int key) {
        int l = 0, r = node.keyCount;
        while (l < r) {
            int m = (l + r) / 2;
            if (node.keys[m] < key) l = m + 1;
            else r = m;
        }
        return l;
    }
    static int upperBound(const IndexNode& node, int key) {
        int l = 0, r = node.keyCount;
        while (l < r) {
            int m = (l + r) / 2;
            if (key >= node.keys[m]) l = m + 1;
            else r = m;
        }
        return l;
    }

    
    static void shiftRightLeaf(IndexNode& leaf, int pos) {
        for (int i = leaf.keyCount; i > pos; --i) {
            leaf.keys[i] = leaf.keys[i - 1];
            leaf.records[i] = leaf.records[i - 1];
        }
    }
    static void shiftLeftLeaf(IndexNode& leaf, int pos) {
        for (int i = pos; i + 1 < leaf.keyCount; ++i) {
            leaf.keys[i] = leaf.keys[i + 1];
            leaf.records[i] = leaf.records[i + 1];
        }
        leaf.keys[leaf.keyCount - 1] = 0;
        leaf.records[leaf.keyCount - 1] = 0;
    }

    static void shiftRightInternal(IndexNode& node, int posKey, int posChild) {
        for (int i = node.keyCount; i > posKey; --i) node.keys[i] = node.keys[i - 1];
        for (int i = node.keyCount + 1; i > posChild; --i) node.children[i] = node.children[i - 1];
    }

    

    
    long long allocateLeaf(IndexNode& nodeOut) {
        IndexNode node;
        node.id = fm.meta.nextNodeId++;
        node.isLeaf = true;
        node.keyCount = 0;
        node.nextLeaf = -1;
        long long off = fm.appendIndexNode(node);
        fm.persistMeta();
        fm.readIndexNode(off, nodeOut);
        return off;
    }

    long long allocateInternal(IndexNode& nodeOut) {
        IndexNode node;
        node.id = fm.meta.nextNodeId++;
        node.isLeaf = false;
        node.keyCount = 0;
        long long off = fm.appendIndexNode(node);
        fm.persistMeta();
        fm.readIndexNode(off, nodeOut);
        return off;
    }

    void splitLeafAndInsert_impl(IndexNode& leaf, int insertKey, const Cliente& c) {
        
        IndexNode rightLeaf;
        long long rightOff = allocateLeaf(rightLeaf);

        
        vector<int> tmpKeys;
        vector<long long> tmpRecs;
        tmpKeys.reserve(leaf.keyCount + 1);
        tmpRecs.reserve(leaf.keyCount + 1);
        for (int i = 0; i < leaf.keyCount; ++i) {
            tmpKeys.push_back(leaf.keys[i]);
            tmpRecs.push_back(leaf.records[i]);
        }
        int pos = lowerBound(leaf, insertKey);
        long long newRecOff = fm.appendRecord(c);
        tmpKeys.insert(tmpKeys.begin() + pos, insertKey);
        tmpRecs.insert(tmpRecs.begin() + pos, newRecOff);

        int total = (int)tmpKeys.size();
        int leftCount = total / 2;
        int rightCount = total - leftCount;

        
        for (int i = 0; i < leftCount; ++i) {
            leaf.keys[i] = tmpKeys[i];
            leaf.records[i] = tmpRecs[i];
        }
        
        for (int i = leftCount; i < MAX_KEYS + 1; ++i) {
            leaf.keys[i] = 0;
            leaf.records[i] = 0;
        }
        leaf.keyCount = leftCount;

        
        for (int i = 0; i < rightCount; ++i) {
            rightLeaf.keys[i] = tmpKeys[leftCount + i];
            rightLeaf.records[i] = tmpRecs[leftCount + i];
        }
        for (int i = rightCount; i < MAX_KEYS + 1; ++i) {
            rightLeaf.keys[i] = 0;
            rightLeaf.records[i] = 0;
        }
        rightLeaf.keyCount = rightCount;

        
        rightLeaf.nextLeaf = leaf.nextLeaf;
        leaf.nextLeaf = rightLeaf.selfOffset;

        fm.writeIndexNode(leaf);
        fm.writeIndexNode(rightLeaf);

        
        int separator = rightLeaf.keys[0];
        insertIntoParent(leaf, separator, rightLeaf);
    }

    void insertIntoParent(IndexNode& leftNode, int key, IndexNode& rightNode) {
        
        if (leftNode.selfOffset == fm.meta.rootOffset) {
            IndexNode newRoot;
            long long rootOff = allocateInternal(newRoot);
            newRoot.keyCount = 1;
            newRoot.keys[0] = key;
            newRoot.children[0] = leftNode.selfOffset;
            newRoot.children[1] = rightNode.selfOffset;
            fm.writeIndexNode(newRoot);
            fm.meta.rootOffset = newRoot.selfOffset;
            fm.persistMeta();
            return;
        }

        
        IndexNode parent;
        long long parentOff = findParentOffset(fm.meta.rootOffset, leftNode.selfOffset);
        fm.readIndexNode(parentOff, parent);

        int pos = upperBound(parent, key);
        shiftRightInternal(parent, pos, pos + 1);
        parent.keys[pos] = key;

        
        int childPos = 0;
        while (childPos <= parent.keyCount && parent.children[childPos] != leftNode.selfOffset) childPos++;
        
        for (int i = parent.keyCount + 1; i > childPos + 1; --i) parent.children[i] = parent.children[i - 1];
        parent.children[childPos + 1] = rightNode.selfOffset;
        parent.keyCount++;

        if (parent.keyCount <= MAX_KEYS) {
            fm.writeIndexNode(parent);
        } else {
            splitInternalAndPropagate(parent);
        }
    }

    long long findParentOffset(long long currentOff, long long childOff) {
        IndexNode cur;
        fm.readIndexNode(currentOff, cur);
        if (cur.isLeaf) return -1;
        for (int i = 0; i <= cur.keyCount; ++i) {
            IndexNode ch;
            fm.readIndexNode(cur.children[i], ch);
            if (ch.selfOffset == childOff) return currentOff;
        }
        
        for (int i = 0; i <= cur.keyCount; ++i) {
            long long res = findParentOffset(cur.children[i], childOff);
            if (res >= 0) return res;
        }
        return -1;
    }

    void splitInternalAndPropagate(IndexNode& node) {
        
        IndexNode right;
        long long rightOff = allocateInternal(right);

        int mid = node.keyCount / 2; 
        int rightCount = node.keyCount - mid - 1;

        
        for (int i = 0; i < rightCount; ++i) right.keys[i] = node.keys[mid + 1 + i];
        right.keyCount = rightCount;

        
        for (int i = 0; i < rightCount + 1; ++i) right.children[i] = node.children[mid + 1 + i];

        int promote = node.keys[mid];

        
        node.keyCount = mid;

        fm.writeIndexNode(node);
        fm.writeIndexNode(right);

        
        insertIntoParent(node, promote, right);
    }

    void fixDeletion(IndexNode& leaf, vector<pair<long long,int>>& path) {
        
        if (leaf.selfOffset == fm.meta.rootOffset) {
            
            return;
        }
        if (leaf.keyCount >= MIN_KEYS) {
            
            updateParentSeparatorAfterLeafChange(leaf, path);
            return;
        }

        
        long long parentOff = findParentOffset(fm.meta.rootOffset, leaf.selfOffset);
        IndexNode parent; fm.readIndexNode(parentOff, parent);

        
        int idx = 0; while (idx <= parent.keyCount && parent.children[idx] != leaf.selfOffset) idx++;
        
        IndexNode sibling;
        int siblingIdx = -1;
        bool useLeft = (idx > 0);
        if (useLeft) { siblingIdx = idx - 1; fm.readIndexNode(parent.children[siblingIdx], sibling); }
        else { siblingIdx = idx + 1; if (siblingIdx <= parent.keyCount) fm.readIndexNode(parent.children[siblingIdx], sibling); else return; }

        
        if (sibling.isLeaf) {
            if (useLeft && sibling.keyCount > MIN_KEYS) {
                
                shiftRightLeaf(leaf, 0);
                leaf.keys[0] = sibling.keys[sibling.keyCount - 1];
                leaf.records[0] = sibling.records[sibling.keyCount - 1];
                leaf.keyCount++;
                sibling.keyCount--;
                fm.writeIndexNode(leaf);
                fm.writeIndexNode(sibling);
                
                parent.keys[siblingIdx] = leaf.keys[0];
                fm.writeIndexNode(parent);
                return;
            } else if (!useLeft && sibling.keyCount > MIN_KEYS) {
                
                leaf.keys[leaf.keyCount] = sibling.keys[0];
                leaf.records[leaf.keyCount] = sibling.records[0];
                leaf.keyCount++;
                shiftLeftLeaf(sibling, 0);
                sibling.keyCount--;
                fm.writeIndexNode(leaf);
                fm.writeIndexNode(sibling);
                parent.keys[idx] = sibling.keys[0];
                fm.writeIndexNode(parent);
                return;
            }
            
            if (useLeft) {
                
                for (int i = 0; i < leaf.keyCount; ++i) {
                    sibling.keys[sibling.keyCount + i] = leaf.keys[i];
                    sibling.records[sibling.keyCount + i] = leaf.records[i];
                }
                sibling.keyCount += leaf.keyCount;
                sibling.nextLeaf = leaf.nextLeaf;
                fm.writeIndexNode(sibling);
                
                removeKeyFromParent(parent, siblingIdx, leaf.selfOffset);
            } else {
                
                for (int i = 0; i < sibling.keyCount; ++i) {
                    leaf.keys[leaf.keyCount + i] = sibling.keys[i];
                    leaf.records[leaf.keyCount + i] = sibling.records[i];
                }
                leaf.keyCount += sibling.keyCount;
                leaf.nextLeaf = sibling.nextLeaf;
                fm.writeIndexNode(leaf);
                removeKeyFromParent(parent, idx, sibling.selfOffset);
            }
        }
    }

    void removeKeyFromParent(IndexNode& parent, int keyPos, long long removedChildOff) {
        
        for (int i = keyPos; i + 1 < parent.keyCount; ++i) parent.keys[i] = parent.keys[i + 1];
        for (int i = keyPos + 1; i + 1 <= parent.keyCount; ++i) parent.children[i] = parent.children[i + 1];
        parent.keyCount--;
        fm.writeIndexNode(parent);

        
        if (parent.selfOffset == fm.meta.rootOffset && parent.keyCount == 0) {
            fm.meta.rootOffset = parent.children[0];
            fm.persistMeta();
        } else if (parent.keyCount < MIN_KEYS && parent.selfOffset != fm.meta.rootOffset) {
            
            long long parentOff = parent.selfOffset;
            fixInternalUnderflow(parentOff);
        }
    }

    void fixInternalUnderflow(long long parentOff) {
        IndexNode parent; if (!fm.readIndexNode(parentOff, parent)) return;
        if (parent.selfOffset == fm.meta.rootOffset) return;
        if (parent.keyCount >= MIN_KEYS) return;

        long long grandOff = findParentOffset(fm.meta.rootOffset, parent.selfOffset);
        if (grandOff < 0) return;
        IndexNode grand; if (!fm.readIndexNode(grandOff, grand)) return;

        
        int pIdx = 0; while (pIdx <= grand.keyCount && grand.children[pIdx] != parent.selfOffset) pIdx++;
        if (pIdx > grand.keyCount) return;

        int siblingIdx = (pIdx > 0) ? pIdx - 1 : pIdx + 1;
        if (siblingIdx < 0 || siblingIdx > grand.keyCount) return;

        IndexNode sibling; if (!fm.readIndexNode(grand.children[siblingIdx], sibling)) return;
        
        if (siblingIdx == pIdx - 1) {
            
            if (sibling.keyCount > MIN_KEYS) {
                
                for (int i = parent.keyCount; i > 0; --i) parent.keys[i] = parent.keys[i - 1];
                for (int i = parent.keyCount + 1; i > 0; --i) parent.children[i] = parent.children[i - 1];

                
                parent.keys[0] = grand.keys[siblingIdx];
                
                parent.children[0] = sibling.children[sibling.keyCount];

                
                grand.keys[siblingIdx] = sibling.keys[sibling.keyCount - 1];

                
                sibling.keyCount--;
                parent.keyCount++;

                fm.writeIndexNode(sibling);
                fm.writeIndexNode(parent);
                fm.writeIndexNode(grand);
                return;
            }
        } else {
            
            if (sibling.keyCount > MIN_KEYS) {
                
                parent.keys[parent.keyCount] = grand.keys[pIdx];
                parent.children[parent.keyCount + 1] = sibling.children[0];

                
                grand.keys[pIdx] = sibling.keys[0];

                
                for (int i = 0; i + 1 < sibling.keyCount; ++i) sibling.keys[i] = sibling.keys[i + 1];
                for (int i = 0; i + 1 <= sibling.keyCount; ++i) sibling.children[i] = sibling.children[i + 1];
                sibling.keyCount--;
                parent.keyCount++;

                fm.writeIndexNode(sibling);
                fm.writeIndexNode(parent);
                fm.writeIndexNode(grand);
                return;
            }
        }

        
        if (sibling.keyCount + parent.keyCount + 1 <= MAX_KEYS) {
            if (siblingIdx == pIdx - 1) {
                
                
                sibling.keys[sibling.keyCount] = grand.keys[siblingIdx];
                sibling.keyCount++;
                
                for (int i = 0; i < parent.keyCount; ++i) {
                    sibling.keys[sibling.keyCount + i] = parent.keys[i];
                }
                
                for (int i = 0; i <= parent.keyCount; ++i) {
                    sibling.children[sibling.keyCount + i] = parent.children[i];
                }
                sibling.keyCount += parent.keyCount;
                fm.writeIndexNode(sibling);
                
                removeKeyFromParent(grand, siblingIdx, parent.selfOffset);
            } else {
                
                
                parent.keys[parent.keyCount] = grand.keys[pIdx];
                parent.keyCount++;
                
                for (int i = 0; i < sibling.keyCount; ++i) {
                    parent.keys[parent.keyCount + i] = sibling.keys[i];
                }
                for (int i = 0; i <= sibling.keyCount; ++i) {
                    parent.children[parent.keyCount + i] = sibling.children[i];
                }
                parent.keyCount += sibling.keyCount;
                fm.writeIndexNode(parent);
                removeKeyFromParent(grand, pIdx, sibling.selfOffset);
            }
        }
    }

    void updateParentSeparatorAfterLeafChange(IndexNode& leaf, vector<pair<long long,int>>& path) {
        
        if (path.empty()) return;
        int firstKey = (leaf.keyCount > 0) ? leaf.keys[0] : INT_MAX;
        
        long long parentOff = findParentOffset(fm.meta.rootOffset, leaf.selfOffset);
        IndexNode parent; fm.readIndexNode(parentOff, parent);
        
        int idx = 0; while (idx <= parent.keyCount && parent.children[idx] != leaf.selfOffset) idx++;
        if (idx > 0) {
            parent.keys[idx - 1] = firstKey;
            fm.writeIndexNode(parent);
        }
    }

public:
    
    static Cliente makeCliente(int cod, const string& nome, int idade, const string& telefone) {
        Cliente c;
        c.codCliente = cod;
        c.idade = idade;
        memset(c.nome, 0, NAME_SIZE);
        memset(c.telefone, 0, PHONE_SIZE);
        strncpy(c.nome, nome.c_str(), NAME_SIZE - 1);
        strncpy(c.telefone, telefone.c_str(), PHONE_SIZE - 1);
        return c;
    }

    
    bool validate(bool verbose = false) {
        if (fm.meta.rootOffset < 0) return true;
        int leafDepth = -1;
        bool ok = validateNode(fm.meta.rootOffset, 0, leafDepth, verbose, INT_MIN, INT_MAX);
        if (!ok) return false;

        
        
        IndexNode node; if (!fm.readIndexNode(fm.meta.rootOffset, node)) return false;
        while (!node.isLeaf) {
            if (!fm.readIndexNode(node.children[0], node)) return false;
        }
        
        int prev = INT_MIN;
        int depth = 0;
        while (true) {
            for (int i = 0; i < node.keyCount; ++i) {
                if (node.keys[i] < prev) {
                    if (verbose) cout << "Erro: ordem global quebrada: " << node.keys[i] << " < " << prev << "\n";
                    return false;
                }
                prev = node.keys[i];
            }
            if (node.nextLeaf < 0) break;
            if (!fm.readIndexNode(node.nextLeaf, node)) return false;
            depth++;
        }
        return true;
    }

    
    void splitLeafAndInsert(IndexNode& leaf, const Cliente& c) {
        splitLeafAndInsert_impl(leaf, c.codCliente, c);
    }

private:
    
    bool validateNode(long long off, int depth, int& leafDepth, bool verbose, int minKey, int maxKey) {
        IndexNode node;
        if (!fm.readIndexNode(off, node)) {
            if (verbose) cout << "Erro: falha ao ler nó em offset " << off << "\n";
            return false;
        }

        
        if (node.selfOffset != off) {
            if (verbose) cout << "Aviso: selfOffset mismatch no nó " << node.id << " (esperado " << off << ")\n";
        }

        if (node.isLeaf) {
            
            for (int i = 1; i < node.keyCount; ++i) {
                if (node.keys[i - 1] > node.keys[i]) {
                    if (verbose) cout << "Erro: chaves desordenadas em folha " << node.id << "\n";
                    return false;
                }
            }
            for (int i = 0; i < node.keyCount; ++i) {
                if (node.keys[i] < minKey || node.keys[i] > maxKey) {
                    if (verbose) cout << "Erro: chave " << node.keys[i] << " fora do intervalo [" << minKey << "," << maxKey << "] na folha " << node.id << "\n";
                    return false;
                }
            }
            if (leafDepth == -1) leafDepth = depth;
            else if (leafDepth != depth) {
                if (verbose) cout << "Erro: folhas em profundidades diferentes (" << leafDepth << " vs " << depth << ")\n";
                return false;
            }
           
            return true;
        } else {
            
            for (int i = 1; i < node.keyCount; ++i) {
                if (node.keys[i - 1] > node.keys[i]) {
                    if (verbose) cout << "Erro: chaves desordenadas em nó interno " << node.id << "\n";
                    return false;
                }
            }
            
            int childCount = node.keyCount + 1;
            
            if (off != fm.meta.rootOffset) {
                if (node.keyCount < MIN_KEYS || node.keyCount > MAX_KEYS) {
                    if (verbose) cout << "Erro: nó interno " << node.id << " tem keyCount=" << node.keyCount << " fora dos limites\n";
                    return false;
                }
            }

            
            for (int i = 0; i < childCount; ++i) {
                int childMin = minKey;
                int childMax = maxKey;
                if (i == 0) childMax = (node.keyCount > 0) ? node.keys[0] - 1 : maxKey;
                else if (i == node.keyCount) childMin = node.keys[node.keyCount - 1];
                else {
                    childMin = node.keys[i - 1];
                    childMax = node.keys[i] - 1;
                }
                if (!validateNode(node.children[i], depth + 1, leafDepth, verbose, childMin, childMax)) return false;
            }
            return true;
        }
    }
};