import numpy as np
from ptio_backend import PCC_IO
import os


def pcwrite(file_path: str,
            points: np.ndarray,
            colors: np.ndarray = None,
            reflectance: np.ndarray = None,
            asAscii: bool = False):
    """
    Write point cloud data to a file.
    :param file_path: Path to the output point cloud file.
    :param points: NumPy array of points (shape: [N, 3]).
    :param colors: Optional NumPy array of colors (shape: [N, 3]).
    :param reflectance: Optional NumPy array of reflectance values (shape: [N, 1]).
    :param asAscii: Whether to write the file in ASCII format.
    """
    pc = PointCloud()
    pc.set_points(points)
    if colors is not None:
        pc.set_colors(colors)
    if reflectance is not None:
        pc.set_reflectance(reflectance)
    if os.path.isdir(os.path.dirname(file_path)):
        os.makedirs(os.path.dirname(file_path), exist_ok=True)
    pc.write(file_path, asAscii)
    return pc


def pcread(file_path: str, attribute=True, return_pc: bool = False) -> tuple:
    """
    Read point cloud data from a file.
    :param file_path: Path to the point cloud file.
    :param attribute: Whether to read color/reflectance.
    :param return_pc: If True, return the PointCloud object.
    :return: A tuple containing points, colors, and reflectance (if requested).
    """
    pc = PointCloud()
    pc.read(file_path, attribute, attribute)
    if return_pc:
        return pc
    if attribute and pc.has_colors() and pc.has_reflectance():
        return pc.points, np.hstack((pc.colors, pc.reflectance))
    if attribute and pc.has_colors():
        return pc.points, pc.colors
    if attribute and pc.has_reflectance():
        return pc.points, pc.reflectance
    return pc.points

class PointCloud:
    def __init__(self):
        self.points = np.empty((0, 3), dtype=np.float64)
        self.colors = np.empty((0, 3), dtype=np.uint8)
        self.reflectance = np.empty((0, 1), dtype=np.uint16)
        self.path = None
        self._pc = PCC_IO()
        self.positionScale = 1.0 # save the point cloud in *positionScale to avoid precision loss

    def read(self, file_path, colors=False, reflectance=False):
        self.clear()
        self._pc.read(file_path,1/self.positionScale)
        self.path = file_path
        self.read_points()
        if colors:
            self.read_colors()
        if reflectance:
            self.read_reflectance()
        if colors:
            return self.points, self.colors
        elif reflectance:
            return self.points, self.reflectance
        else:
            return self.points

    def write(self, file_path, asAscii=False):
        self._pc.write(file_path, self.positionScale, asAscii)

    def read_points(self):
        self.points = self._pc.get_points().reshape(-1,3).astype(np.float64)
        return self.points

    def read_colors(self):
        if self._pc.has_colors():
            self.colors = self._pc.get_colors().reshape(-1,3)
        return self.colors

    def read_reflectance(self):
        if self._pc.has_reflectance():
            self.reflectance = self._pc.get_reflectance().reshape(-1,1)
        return self.reflectance

    def clear(self):
        self._pc.clear()
        self._pc.remove_colors()
        self._pc.remove_reflectance()

    def set_points(self, points):
        points = (points).astype(np.float64)
        points = np.ascontiguousarray(points)
        self._pc.set_points(points.reshape(-1))

    def set_reflectance(self, reflectance):
        reflectance = reflectance.astype(np.uint16)
        reflectance = np.ascontiguousarray(reflectance)
        self._pc.set_reflectance(reflectance.reshape(-1))

    def set_colors(self, colors):
        colors = colors.astype(np.uint8)
        colors = np.ascontiguousarray(colors)
        self._pc.set_colors(colors.reshape(-1))

    def has_colors(self):
        return self._pc.has_colors()

    def has_reflectance(self):
        return self._pc.has_reflectance()


if __name__ == "__main__":
    # Example usage
    points = np.random.rand(10, 3) * 100  # Random points in 3D space
    colors = np.random.randint(0, 256, (10, 3), dtype=np.uint8)  # Random colors
    reflectance = np.random.randint(0, 65536, (10, 1), dtype=np.uint16)  # Random reflectance values
    print('write')
    print(points, colors, reflectance)
    pcwrite("example.ply", points, colors, reflectance, asAscii=True)
    points_read, attri_read = pcread("example.ply", attribute=True)
    print('read')
    print(points_read, attri_read)
