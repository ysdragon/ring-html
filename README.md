<div align="center">

<img src="img/logo.png" alt="RingHTML Logo" width="200">

# RingHTML

**A high-performance HTML5 parser with CSS selectors and DOM manipulation for the Ring programming language.**

[language-ring]: https://img.shields.io/badge/language-Ring-2D54CB.svg?style=for-the-badge&labelColor=414868
[last-commit]: https://img.shields.io/github/last-commit/ysdragon/ring-html?style=for-the-badge&logo=github&logoColor=C0CAF5&labelColor=414868&color=8c73cc
[release-version]: https://img.shields.io/badge/dynamic/regex?url=https%3A%2F%2Fraw.githubusercontent.com%2Fysdragon%2Fring-html%2Fmaster%2Fpackage.ring&search=%3Aversion\s*%3D\s*%22([^%22]%2B)%22&replace=%241&style=for-the-badge&label=version&labelColor=414868&color=7664C6
[license]: https://img.shields.io/github/license/ysdragon/ring-html?style=for-the-badge&logo=opensourcehardware&label=License&logoColor=C0CAF5&labelColor=414868&color=8c73cc
[issues]: https://img.shields.io/github/issues/ysdragon/ring-html?style=for-the-badge&logo=github&logoColor=C0CAF5&labelColor=414868&color=8c73cc

[![][language-ring]](https://ring-lang.github.io/)
[![][last-commit]](https://github.com/ysdragon/ring-html/pulse)
[![][release-version]](https://github.com/ysdragon/ring-html/releases/latest)
[![][license]](https://github.com/ysdragon/ring-html/blob/main/LICENSE)
[![][issues]](https://github.com/ysdragon/ring-html/issues)

</div>

RingHTML is a powerful Ring library for parsing and manipulating HTML documents. It provides a simple and intuitive API for extracting data, navigating the DOM tree, and building HTML programmatically.

This project is made possible by the blazing-fast [Lexbor](https://github.com/lexbor/lexbor) HTML5 parser.

## ✨ Features

- **High-Performance:** Powered by Lexbor, one of the fastest HTML5 parsers available.
- **CSS Selectors:** Full support for CSS selectors (`#id`, `.class`, `tag`, `parent child`, etc.).
- **DOM Navigation:** Traverse parent, children, siblings, first/last child with ease.
- **DOM Manipulation:** Create, modify, insert, and remove nodes programmatically.
- **Content Extraction:** Extract text, HTML, innerHTML, and attributes from any element.
- **Unicode Support:** Full international character support for multilingual content.

## 🚀 Getting Started

Follow these instructions to get the RingHTML library up and running on your system.

### Prerequisites

- **[Ring](https://ring-lang.github.io/download.html):** Ensure you have Ring language version 1.24 or higher installed.

### Installation

#### Install using Ring Package Manager (**RingPM**)

```sh
ringpm install ring-html from ysdragon
```

## 💻 Usage

Parsing HTML and extracting data is straightforward. Here's a simple example:

```ring
# Load the RingHTML library
load "html.ring"

# Parse an HTML document
doc = new HTML('
<html>
<body>
    <h1 class="title">Welcome to RingHTML!</h1>
    <div class="content">
        <p>This is a <strong>powerful</strong> HTML parser.</p>
        <ul>
            <li><a href="/docs">Documentation</a></li>
            <li><a href="/examples">Examples</a></li>
        </ul>
    </div>
</body>
</html>
')

# Find elements using CSS selectors
title = doc.find("h1.title")[1]
see "Title: " + title.text() + nl

# Extract all links
links = doc.find("a")
for link in links
    see "Link: " + link.text() + " -> " + link.attr("href") + nl
next
```

For more advanced examples, see the [`examples/`](examples/) directory.

## 📚 API Reference

### HTML Class

The main document parser class.

| Method | Description |
|--------|-------------|
| `new HTML(html)` | Parse HTML string and create document |
| `find(selector)` | Find all elements matching CSS selector → `[HTMLNode, ...]` |
| `findAttr(selector, attrName)` | Find all elements matching CSS selector and return attribute values → `[string, ...]` |
| `body()` | Get the `<body>` element → `HTMLNode` |
| `head()` | Get the `<head>` element → `HTMLNode` |
| `root()` / `html()` | Get the `<html>` root element → `HTMLNode` |
| `createNode(tagName)` | Create a new element → `HTMLNode` |
| `createTextNode(text)` | Create a new text node → `HTMLNode` |

### HTMLNode Class

Represents a DOM node with full navigation and manipulation capabilities.

#### Content Extraction

| Method | Description |
|--------|-------------|
| `text()` | Get combined text content of node and children |
| `html()` | Get outer HTML (includes the tag itself) |
| `innerHTML()` | Get inner HTML (children only) |
| `tag()` | Get tag name (e.g., `"div"`, `"p"`) |

#### Attributes

| Method | Description |
|--------|-------------|
| `attr(name)` | Get attribute value |
| `has_attr(name)` | Check if attribute exists → `bool` |
| `attributes()` | Get all attributes → `[[name, value], ...]` |
| `setAttribute(name, value)` | Set or update attribute |
| `removeAttribute(name)` | Remove an attribute |

#### Navigation

| Method | Description |
|--------|-------------|
| `find(selector)` | Find descendants matching selector |
| `findAttr(selector, attrName)` | Find descendants matching selector and return attribute values → `[string, ...]` |
| `parent()` | Get parent node |
| `children()` | Get all child nodes → `[HTMLNode, ...]` |
| `firstChild()` | Get first child element |
| `lastChild()` | Get last child element |
| `next_sibling()` | Get next sibling node |
| `prev_sibling()` | Get previous sibling node |

#### Manipulation

| Method | Description |
|--------|-------------|
| `appendChild(node)` | Append a child node |
| `insertBefore(node)` | Insert node before this one |
| `insertAfter(node)` | Insert node after this one |
| `remove()` | Remove this node from DOM |
| `setInnerText(text)` | Set text content, replacing all children |
| `setInnerHTML(html)` | Set inner HTML, replacing all children |

## 🛠️ Development

If you want to contribute to the development of RingHTML or build it from source, follow these steps.

### Prerequisites

- **CMake:** Version 3.16 or higher.
- **C Compiler:** A C compiler compatible with your platform (e.g., GCC, Clang, MSVC).
- **[Ring](https://github.com/ring-lang/ring):** You need to have the Ring language source code available on your machine.

### Build Steps

1. **Clone the Repository:**
   ```sh
   git clone https://github.com/ysdragon/ring-html.git --recursive
   cd ring-html
   ```

2. **Set the `RING` Environment Variable:**
   Before running CMake, set the `RING` environment variable to point to the Ring language source code.
   - **Windows (Command Prompt):**
     ```cmd
     set RING=X:\path\to\ring
     ```
   - **Windows (PowerShell):**
     ```powershell
     $env:RING = "X:\path\to\ring"
     ```
   - **Unix/Linux/macOS:**
     ```bash
     export RING=/path/to/ring
     ```

3. **Configure with CMake:**
   ```sh
   mkdir build
   cd build
   cmake ..
   ```

4. **Build the Project:**
   ```sh
   cmake --build .
   ```

The compiled library will be placed in the `lib/<os>/<arch>` directory.

## Platform Support

| Platform | Architectures |
|----------|---------------|
| Linux | amd64, arm64 |
| macOS | amd64, arm64 |
| FreeBSD | amd64, arm64 |
| Windows | x64, x86, ARM64 |

## 🤝 Contributing

Contributions are welcome! If you have ideas for improvements or have found a bug, please open an issue or submit a pull request.

## 📄 License

This project is licensed under the MIT License. See the [`LICENSE`](LICENSE) file for details.