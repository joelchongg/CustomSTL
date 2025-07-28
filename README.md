# Custom STL
**Custom STL** is a personal C++ project focused on reimplementing and optimizing core components of the Standard Template Library (STL), with a focus on performance, memory efficiency, and systems-level design.

## 🚀 Project Purpose
This project is intended as a deep dive into the internals of STL containers and memory management in modern C++. It's built to:

- Strengthen my understanding of template metaprogramming, move semantics, allocators, and iterators
- Explore performance trade-offs in container design
- Develop low-level problem-solving and systems programming skills in modern C++

## 🔧 Planned Components

- `customSTL::unique_ptr` - lightweight exclusive ownership smart pointer
- `customSTL::shared_ptr<T>` – reference-counted smart pointer
- `customSTL::vector<T>` – dynamic array with allocator support and move semantics
- `customSTL::string` – custom string class with small string optimization (SSO)
- `customSTL::allocator<T>` - custom memory allocator interface for containers
- `customSTL::unordered_map<K, V>` – hash table with custom probing
- `customSTL::map<K, V>` – balanced binary tree (e.g., red-black or AVL)
- Custom memory allocators, benchmarking tools, and more

All components will be:
- **Header-only**
- **Fully unit-tested**
- **Written in modern C++23 or later**

## 📈 Why This Project?

As an aspiring software engineer with a strong interest in high-performance computing and system-level development, I'm using this project to explore the core building blocks of the C++ Standard Library. Reimplementing STL containers from scratch is a challenging but rewarding way to develop deeper insights into how C++ really works under the hood.

For others reading this, I hope you find inspiration in the design decisions or gain a deeper understanding of how STL components are typically implemented (perhaps even take a shot at implementing it yourself!).

## ⚙️ Build Instructions (Coming Soon)

This project will use CMake for building and testing. Example usage will be included soon.

## 📝 License

This project is licensed under the MIT License — see [`LICENSE`](./LICENSE) for details.