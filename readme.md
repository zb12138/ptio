# Fast point cloud I/O library

## Overview

This is a Python package for processing 3D point cloud data. It provides functionality for reading and writing PLY files with colors and reflectance. It is lighter and faster than most point cloud IO packages (e.g., opend3d, plyfile).

## Installation

You can install the package using the following command:

```bash
pip install git+https://github.com/zb12138/ptio.git
```


## Usage Example
Below is an example demonstrating how to generate random point cloud data, write it to a PLY file, and then read that file back.

```python
import numpy as np
from ptio import pcwrite, pcread  

if __name__ == "__main__":
    # Generate random points
    points = np.random.rand(10, 3) * 100  # Random points in 3D space
    colors = np.random.randint(0, 256, (10, 3), dtype=np.uint8)  # Random colors
    reflectance = np.random.randint(0, 65536, (10, 1), dtype=np.uint16)  # Random reflectance values
    
    print('Writing data')
    print(points, colors, reflectance)
    
    # Write to PLY file
    pcwrite("example.ply", points, colors, reflectance, asAscii=True)
    
    # Read from PLY file
    points_read, attri_read = pcread("example.ply", attribute=True)
    print('Reading data')
    print(points_read, attri_read)
```

## Contributing
Contributions are welcome! Please submit a pull request or open an issue to discuss.

## License
This project is licensed under the BSD License. See the LICENSE file for details.