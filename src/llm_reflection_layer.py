# reflector.py using Ollama
import sys
import requests

prompt = sys.stdin.read().strip()
if not prompt:
    print("[ERROR] No prompt received.", file=sys.stderr)
    sys.exit(1)

response = requests.post("http://localhost:11434/api/generate", json={
    "model": "mistral",  
    "prompt": prompt,
    "stream": False
})

if response.status_code == 200:
    result = response.json()["response"]
    print(result.strip())
else:
    print("[ERROR] LLM request failed:", response.text, file=sys.stderr)
