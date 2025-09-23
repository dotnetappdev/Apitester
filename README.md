# API Tester

A modern Postman-like desktop application for API testing built with C++ and Qt.

![API Tester Logo](resources/icons/app.ico)

## Features

✅ **HTTP Methods Support**
- GET, POST, PUT, DELETE, PATCH, HEAD, OPTIONS

✅ **Request Configuration**
- Custom headers with enable/disable toggles
- Multiple body types (raw, JSON, form-data, x-www-form-urlencoded)
- URL parameters with enable/disable toggles
- Authentication support (Basic Auth, Bearer Token, API Key)

✅ **Response Handling**
- Formatted JSON response viewer with syntax highlighting
- JSON tree structure viewer
- Raw response viewer
- Response headers display
- Status code, response time, and size information

✅ **Collections Management**
- Organize requests into collections
- Save and load requests
- CRUD operations for collections and requests
- Request history tracking

✅ **User Interface**
- Modern Qt-based interface
- Dark and light theme support
- Multiple request tabs
- Resizable panels with splitters
- Keyboard shortcuts

✅ **Data Persistence**
- SQLite database for local storage
- Automatic request history
- Collections and requests saved locally

## Technology Stack

- **Language**: C++17
- **Framework**: Qt5
- **Database**: SQLite
- **HTTP Client**: Qt Network (QNetworkAccessManager)
- **Build System**: CMake

## Requirements

- Qt5 (Core, Widgets, Network, Sql)
- CMake 3.16+
- C++17 compatible compiler (GCC, Clang, MSVC)

## Building

### Linux/Ubuntu

```bash
# Install Qt5 development packages
sudo apt update
sudo apt install qtbase5-dev qtchooser qt5-qmake qtbase5-dev-tools \
                 libqt5sql5-sqlite libqt5network5

# Build the application
mkdir build && cd build
cmake ..
make -j$(nproc)
```

### Running

```bash
cd build
./ApiTester
```

## Usage

1. **Creating Collections**: Use the "New Collection" button to organize your requests
2. **Adding Requests**: Select a collection and click "New Request" to add API endpoints
3. **Configuring Requests**:
   - Select HTTP method from dropdown
   - Enter the request URL
   - Add headers in the Headers tab
   - Configure request body in the Body tab
   - Set URL parameters in the Parameters tab
   - Configure authentication in the Auth tab
4. **Sending Requests**: Click the "Send" button to execute the request
5. **Viewing Responses**: Response data appears in the right panel with multiple view options
6. **Switching Themes**: Use Ctrl+T or the View menu to toggle between dark and light themes

## Screenshots

*Screenshots will be added after testing the application*

## Project Structure

```
Apitester/
├── src/                    # Source code
│   ├── main.cpp           # Application entry point
│   ├── MainWindow.*       # Main application window
│   ├── RequestPanel.*     # HTTP request configuration
│   ├── ResponsePanel.*    # Response display and formatting
│   ├── CollectionManager.*# Collections and requests management
│   ├── DatabaseManager.*  # SQLite database operations
│   ├── ThemeManager.*     # Dark/light theme management
│   ├── HttpClient.*       # HTTP request handling
│   └── *Model.*          # Data models
├── resources/             # Application resources
│   ├── resources.qrc     # Qt resource file
│   └── icons/            # Application icons
├── CMakeLists.txt        # Build configuration
└── README.md            # This file
```

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Build and test the application
5. Submit a pull request

## License

This project is open source. Feel free to use and modify as needed.

## Similar Projects

This project aims to provide functionality similar to:
- Postman
- Insomnia
- HTTPie Desktop
- Advanced REST Client