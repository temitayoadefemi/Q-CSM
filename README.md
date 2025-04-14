
# Q-CSM: Quantum-Inspired Cognitive State Machine

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT) 

## Overview

Q-CSM is an experimental C++/Python project exploring a novel approach to modeling cognitive states and reflection. It uses a quantum-inspired framework where potential thoughts exist in a superposition, influenced by simulated emotions, and ultimately resolve into a state that is interpreted and reflected upon by a Large Language Model (LLM).

## Core Concepts

* **Quantum Inspiration:** Uses analogies from quantum mechanics (superposition, complex amplitudes, normalization, collapse/evolution) to represent uncertainty and potentiality in cognitive states. This is purely *analogical*, not actual quantum computation.
* **QBrainState:** A state represented as a superposition (`std::vector<QState>`) of basis states, where each basis state holds a semantic embedding vector and a complex amplitude.
* **Embeddings:** Input sentences/thoughts are converted into vector embeddings using Sentence Transformers (`all-MiniLM-L6-v2`) via a Python Flask server.
* **Emotion Modulation:** A simulated `EmotionVector` can bias the probabilities (|amplitude|^2) of the different basis states within the superposition.
* **Decision Layer:** The superposition can be resolved either by:
    * **Collapse:** Probabilistically selecting a single basis state based on its amplitude.
    * **Evolution:** Calculating a weighted average vector representing the overall cognitive direction.
* **Concept Mapping:** The resulting vector (from collapse or evolution) is compared to a library of known "concept" vectors to identify dominant themes.
* **LLM Reflection:** The identified top concepts are used to generate a prompt for an LLM (e.g., Mistral via Ollama), which then generates a natural language reflection on the simulated internal state.

## Architecture

1.  **Input:** Sentences provided via command line.
2.  **Embedding Service (Python/Flask):** `embedding_server.py` serves vector embeddings for input text via HTTP.
3.  **Q-CSM Core (C++):**
    * `main.cpp`: Orchestrates the process, handles CLI args.
    * `network_utils`: Handles communication with the embedding server (via libcurl).
    * `q-csm/`: Library implementing `QState`, `QBrainState`, generation, normalization, emotion engine, decision layer, and concept mapping.
4.  **LLM Reflector (Python):** `reflector.py` takes a generated prompt, queries a local LLM service (Ollama), and returns the response.

## Requirements

* C++17 Compiler (like g++, clang++)
* CMake (version 3.15+) or Make
* libcurl library and headers
* nlohmann/json library (header-only usually sufficient)
* Python 3.x
* Python libraries: `flask`, `sentence-transformers`, `requests`
* An Ollama instance running with a suitable model (e.g., `mistral`) accessible at `http://localhost:11434`.
* An embedding server instance running (from `embedding_server.py`) accessible at `http://localhost:5005`.

## Installation & Setup

1.  **Clone the repository:**
    ```bash
    git clone <your-repo-url>
    cd <your-repo-name>
    ```
2.  **Install Python dependencies:**
    ```bash
    pip install -r requirements.txt # Make sure you create a requirements.txt!
    # (requirements.txt should contain: flask, sentence-transformers, requests)
    ```
3.  **Build the C++ application:**
    * **Using CMake (Recommended):**
        ```bash
        mkdir build
        cd build
        cmake .. # Adjust paths in CMakeLists.txt if curl/json are not found automatically
        make
        cd ..
        # Executable will be in build/q_csm_reflector
        ```
    * **Using Makefile:**
        ```bash
        make # Adjust paths/libs in Makefile if needed
        # Executable will be ./q_csm_reflector
        ```

## Usage

1.  **Start the Embedding Server:**
    ```bash
    python embedding_server.py
    ```
    *(Keep this running in a separate terminal)*

2.  **Start Ollama Service:** Ensure your Ollama service with the required model (e.g., `mistral`) is running.

3.  **Run the Q-CSM Reflector:**
    * If using CMake:
        ```bash
        ./build/q_csm_reflector "Input sentence 1." "Another thought or feeling." "A third perspective."
        ```
    * If using Makefile:
        ```bash
        ./q_csm_reflector "Input sentence 1." "Another thought or feeling." "A third perspective."
        ```
    * The program will connect to the embedding server, process the inputs through the Q-CSM core, connect to the LLM reflector, and print the initial state, modulated state (if implemented), decision, and final LLM reflection.

## Configuration

* Currently, URLs for the embedding server (`http://localhost:5005/embed`) and LLM (`http://localhost:11434/api/generate`), the LLM model name (`mistral`), and embedding model name (`all-MiniLM-L6-v2`) are hardcoded. Future improvements could involve using configuration files or command-line arguments.
* Emotion vector parameters are currently hardcoded or simplified in `main.cpp`.

