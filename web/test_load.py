import sys
import os
sys.path.append(os.getcwd())
try:
    from core_bridge import SimilarityEngine
    print("Import successful")
    engine = SimilarityEngine()
    print("Engine created")
except Exception as e:
    print(f"Error: {e}")
    import traceback
    traceback.print_exc()
