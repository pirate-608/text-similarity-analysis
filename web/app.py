from flask import Flask, render_template, request, jsonify
import os
import tempfile
import shutil
from core_bridge import SimilarityEngine

app = Flask(__name__)
engine = SimilarityEngine()

UPLOAD_FOLDER = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'uploads')
if not os.path.exists(UPLOAD_FOLDER):
    os.makedirs(UPLOAD_FOLDER)

@app.route('/')
def index():
    return render_template('index.html')

import traceback

@app.route('/analyze', methods=['POST'])
def analyze():
    try:
        if 'files[]' not in request.files:
            return jsonify({'error': 'No files uploaded'}), 400
        
        files = request.files.getlist('files[]')
        
        # Create a unique temporary directory for this request
        session_id = os.urandom(8).hex()
        session_dir = os.path.join(UPLOAD_FOLDER, session_id)
        os.makedirs(session_dir)
        
        try:
            saved_files = []
            for file in files:
                if file.filename == '':
                    continue
                if file.filename.endswith('.txt'):
                    filepath = os.path.join(session_dir, file.filename)
                    file.save(filepath)
                    saved_files.append(file.filename)
            
            if not saved_files:
                return jsonify({'error': 'No valid text files uploaded'}), 400
                
            # Call C engine
            result = engine.process_directory(session_dir)
            
            if not result:
                return jsonify({'error': 'Analysis failed'}), 500
                
            return jsonify(result)
            
        finally:
            # Cleanup
            if os.path.exists(session_dir):
                shutil.rmtree(session_dir)
    except Exception as e:
        traceback.print_exc()
        return jsonify({'error': str(e)}), 500

if __name__ == '__main__':
    app.run(host='0.0.0.0', debug=True, port=5000)
