# SurgicalWiper

## ⚠️ IMPORTANT: EDUCATIONAL PURPOSE ONLY

**This project is a Proof-of-Concept (PoC) developed strictly for academic and research purposes. It is intended for use in controlled, ethical security research environments to study malware behavior and improve defensive solutions.**

## Overview

This project demonstrates a paradigm shift in anti-forensics known as **Surgical Data Sanitization**.

Traditional wiping tools and ransomware methodologies rely on brute-force execution. They overwrite every addressable block of a file. While effective, this approach is noisy. It generates massive I/O signatures, spikes CPU usage, and dramatically alters file entropy, making it trivially detectable by Endpoint Detection and Response (EDR) systems and Heuristic AVs.

**This PoC is different.** Instead of destroying the data, it destroys the *logic*, or the grammar, required to interpret it.

By targeting specific "structural keystones," such as the Document Catalog in PDFs or the Huffman Tables in DOCX files, this tool renders files irretrievable while overwriting less than **0.01%** of the file's content.

## Evasion & Detection Analysis

**VirusTotal Detection Rate: 2 / 70** (At time of testing)

The exceptionally low detection rate is not a bug. It is the architectural feature of this approach. Most AVs rely on specific heuristics that this PoC intentionally avoids:

1.  **I/O Invisibility:** Security monitoring often flags processes that write megabytes of data in short bursts. This wiper modifies only \~50-100 bytes per file on average.
2.  **Entropy Preservation:** Ransomware & Wiper samples usually drastically change the entropy of each touched file. Because we only touch metadata headers, the overall entropy of the file remains statistically unchanged, bypassing entropy-based heuristics.
3.  **The "Corrupted" Masquerade:** The files are not deleted. They remain on the disk with their original size and headers intact. To the OS, they look healthy. To the parser, they are dead.

## Technical Approach

The core of the project utilizes a Strategy Pattern in C++ (`IWiperStrategy`) to apply format-specific destruction techniques.

### 1\. PDF Strategy: Severing the Graph

  * **The Target:** The Portable Document Format is a graph of objects, not a linear stream. The critical node is the **Document Catalog** (`/Type /Catalog`) which points to the Page Tree.
  * **The Technique:** The code scans the binary for the dictionary defining the Catalog and overwrites the dictionary body (between `<<` and `>>`) with a hardcoded byte.
  * **The Result:** Without the Root object, PDF readers and forensic tools cannot build the reference table. The document becomes a "bag of orphaned objects" with no page order or structure.

### 2\. DOCX Strategy: The Huffman Attack

  * **The Target:** A DOCX file is a ZIP archive containing XML. The text resides in `word/document.xml`, which is stored as a Deflate compressed stream.
  * **The Technique:**
      * **Anti-Carving:** The tool overwrites the filename `word/document.xml` in the local ZIP header to prevent recovery by name.
      * **Anti-Decompression:** It overwrites the first **50 bytes** of the Deflate payload.
  * **The Result:** The Deflate algorithm relies on dynamic Huffman codes defined at the *start* of the stream. By destroying this header, the remaining compressed data becomes an undecodable bit-soup. The XML is recoverable only via infeasible brute-forcing of the Huffman tree.

## Project Structure

```text
.
├── main.cpp              # Entry point; defines target directory
├── FileProcessor.cpp     # Recursively scans directories and identifies file types
├── PdfStrategy.cpp       # Implements the "Graph Severing" logic for .pdf
├── DocxStrategy.cpp      # Implements the "Huffman Attack" for .docx
├── IWiperStrategy.h      # Interface for adding new format strategies
└── WipeInterval.h        # Struct defining the offset/length for surgical strikes
```

## TODO & Future Research

The current PoC implements the foundational logic for document formats. Future iterations will focus on the following:

  * **[ ] Expand Format Support:** Research and implement strategies for additional file formats (such as PNG, JPG, and MP4) by identifying and targeting their specific structural vulnerabilities.
  * **[ ] Randomization:** Replace the hardcoded overwrite char with cryptographically secure random bytes to further frustrate pattern-based signature detection.
