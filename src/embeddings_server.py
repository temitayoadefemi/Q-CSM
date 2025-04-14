from sentence_transformers import SentenceTransformer
from flask import Flask, request, jsonify
import logging 


logging.basicConfig(level=logging.INFO)

app = Flask(__name__)


MODEL_NAME = "all-MiniLM-L6-v2"
try:
    logging.info(f"Loading sentence transformer model: {MODEL_NAME}...")
    model = SentenceTransformer(MODEL_NAME)
    logging.info("Model loaded successfully.")
except Exception as e:
    logging.error(f"Failed to load model '{MODEL_NAME}': {e}")
    model = None 


# --- API Endpoint ---
@app.route('/embed', methods=['POST'])
def embed():
    if model is None:
        return jsonify({"error": "Model not loaded"}), 503 
    try:
        data = request.get_json()
        if data is None:
            return jsonify({"error": "Request body must be JSON with Content-Type: application/json"}), 400
    except Exception as e:
        logging.error(f"Error parsing JSON request: {e}")
        return jsonify({"error": "Failed to parse JSON request"}), 400

    sentence = data.get("sentence")
    if not sentence or not isinstance(sentence, str):
        return jsonify({"error": "Missing or invalid 'sentence' field in JSON body"}), 400

    try:
        logging.info(f"Encoding sentence: '{sentence[:50]}...'")
        embedding = model.encode(sentence)
        embedding_list = embedding.tolist()
        logging.info(f"Encoding successful, embedding size: {len(embedding_list)}")
        return jsonify({"embedding": embedding_list})

    except Exception as e:
        logging.error(f"Error during sentence encoding: {e}")
        return jsonify({"error": "Failed to encode sentence"}), 500

# --- Main Execution ---
if __name__ == '__main__':
    logging.info("Starting Flask development server...")
    app.run(host="0.0.0.0", port=5005, debug=False) 