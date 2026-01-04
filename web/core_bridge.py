import ctypes
import os
import sys

# Define structures
class HashTable(ctypes.Structure):
    _fields_ = [] # We don't need to access internals of HashTable in Python usually

class Document(ctypes.Structure):
    _fields_ = [
        ("filename", ctypes.c_char * 256),
        ("word_freq", ctypes.POINTER(HashTable)),
        ("content", ctypes.c_char_p),
        ("word_count", ctypes.c_size_t)
    ]

class DocumentCollection(ctypes.Structure):
    _fields_ = [
        ("documents", ctypes.POINTER(ctypes.POINTER(Document))),
        ("count", ctypes.c_size_t),
        ("capacity", ctypes.c_size_t)
    ]

class SimilarityMatrix(ctypes.Structure):
    _fields_ = [
        ("matrix", ctypes.POINTER(ctypes.POINTER(ctypes.c_double))),
        ("filenames", ctypes.POINTER(ctypes.c_char_p)),
        ("size", ctypes.c_size_t)
    ]

class StopWords(ctypes.Structure):
    _fields_ = [
        ("words", ctypes.POINTER(ctypes.c_char_p)),
        ("size", ctypes.c_size_t),
        ("capacity", ctypes.c_size_t)
    ]

# Load Library
def load_lib():
    # Assuming the DLL is in ../build/bin/libsimilarity.dll relative to this file
    base_path = os.path.dirname(os.path.abspath(__file__))
    dll_path = os.path.join(base_path, "..", "build", "bin", "libsimilarity.dll")
    
    if not os.path.exists(dll_path):
        raise FileNotFoundError(f"DLL not found at {dll_path}")
        
    lib = ctypes.CDLL(dll_path)
    
    # Define return types and argument types
    
    # StopWords* stop_words_create();
    lib.stop_words_create.restype = ctypes.POINTER(StopWords)
    
    # void stop_words_destroy(StopWords *sw);
    lib.stop_words_destroy.argtypes = [ctypes.POINTER(StopWords)]
    
    # DocumentCollection* load_documents_from_dir(const char *dir_path, StopWords *stop_words);
    lib.load_documents_from_dir.restype = ctypes.POINTER(DocumentCollection)
    lib.load_documents_from_dir.argtypes = [ctypes.c_char_p, ctypes.POINTER(StopWords)]
    
    # void collection_destroy(DocumentCollection *col);
    lib.collection_destroy.argtypes = [ctypes.POINTER(DocumentCollection)]
    
    # SimilarityMatrix* similarity_matrix_create(DocumentCollection *col);
    lib.similarity_matrix_create.restype = ctypes.POINTER(SimilarityMatrix)
    lib.similarity_matrix_create.argtypes = [ctypes.POINTER(DocumentCollection)]
    
    # void similarity_matrix_destroy(SimilarityMatrix *matrix);
    lib.similarity_matrix_destroy.argtypes = [ctypes.POINTER(SimilarityMatrix)]
    
    return lib

class SimilarityEngine:
    def __init__(self):
        self.lib = load_lib()
        self.stop_words = self.lib.stop_words_create()
        
    def __del__(self):
        if hasattr(self, 'lib') and hasattr(self, 'stop_words'):
            self.lib.stop_words_destroy(self.stop_words)
            
    def process_directory(self, dir_path):
        dir_path_bytes = dir_path.encode('utf-8')
        collection = self.lib.load_documents_from_dir(dir_path_bytes, self.stop_words)
        
        if not collection:
            return None
            
        matrix = self.lib.similarity_matrix_create(collection)
        
        result = {
            "filenames": [],
            "matrix": []
        }
        
        if matrix:
            size = matrix.contents.size
            # Read filenames
            for i in range(size):
                raw_name = matrix.contents.filenames[i]
                try:
                    # Try UTF-8 first
                    decoded_name = raw_name.decode('utf-8')
                except UnicodeDecodeError:
                    try:
                        # Fallback to GBK (common for Chinese Windows)
                        decoded_name = raw_name.decode('gbk')
                    except UnicodeDecodeError:
                        # Fallback to system default or ignore errors
                        decoded_name = raw_name.decode('mbcs', errors='replace') if os.name == 'nt' else raw_name.decode('utf-8', errors='replace')
                
                result["filenames"].append(decoded_name)
            
            # Read matrix
            for i in range(size):
                row = []
                row_ptr = matrix.contents.matrix[i]
                for j in range(size):
                    row.append(row_ptr[j])
                result["matrix"].append(row)
                
            self.lib.similarity_matrix_destroy(matrix)
            
        self.lib.collection_destroy(collection)
        return result
