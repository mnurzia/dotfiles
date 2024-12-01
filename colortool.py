from typing import Self, NamedTuple, Protocol, runtime_checkable
import heapq


class ColorSpace:
    _REGISTRY: dict[str, type[Self]] = dict()
    _PREFIX = "to"

    @runtime_checkable
    class ConversionFunc(Protocol):
        @staticmethod
        def __call__(x: "ColorSpace") -> "ColorSpace": ...

    class Conversion(NamedTuple):
        dst: str
        f: "ColorSpace.ConversionFunc"
        weight: float = 0

    _CONVERSIONS: dict[str, list[Conversion]] = {}

    fields = dict[str, type]

    def __init__(self, *args): ...

    def __init_subclass__(cls, /, auto=True, **kwargs) -> None:
        super().__init_subclass__(**kwargs)
        id = cls.__name__
        ColorSpace._REGISTRY[id] = cls
        if auto:
            cls.fields = cls.__annotations__

            def __init__(self, *args):
                for field_name, field_value in zip(self.fields.keys(), args):
                    setattr(self, field_name, field_value)

            cls.__init__ = __init__

            functions = [
                (n[len(ColorSpace._PREFIX) :], f)
                for (n, f) in cls.__dict__.items()
                if n.startswith(ColorSpace._PREFIX)
                and isinstance(f, ColorSpace.ConversionFunc)
            ]

            ColorSpace._CONVERSIONS[id] = []
            for name, function in functions:
                ColorSpace._CONVERSIONS[id].append(
                    ColorSpace.Conversion(name, function)
                )

    def find(self, id: str) -> type[Self]:
        return self._REGISTRY[id]

    @classmethod
    def plan(cls, src: str, dst: str) -> tuple[ConversionFunc, ...]:
        prev: dict[str, tuple[str, ColorSpace.ConversionFunc]] = {}

        class Identity(ColorSpace.ConversionFunc):
            @staticmethod
            def __call__(x: ColorSpace) -> ColorSpace:
                return x

        queue: list[tuple[float, str, str, ColorSpace.ConversionFunc]] = [
            (0.0, src, src, Identity())
        ]
        while len(queue):
            top_dist, top, top_prev, top_func = heapq.heappop(queue)
            if top in prev:
                continue
            prev[top] = (top_prev, top_func)
            if top == dst:
                break
            for edge in sorted(
                cls._CONVERSIONS[top], key=lambda edge: edge.weight, reverse=True
            ):
                heapq.heappush(queue, (top_dist + edge.weight, edge.dst, top, edge.f))
        cvts: list[ColorSpace.ConversionFunc] = []
        current = dst
        while current != src:
            if current not in prev:
                raise ValueError(f"no path from {src} to {dst}")
            next, func = prev[current]
            cvts.append(func)
            current = next
        return tuple(cvts)


class xyY(ColorSpace):
    x: float
    y: float
    Y: float

    def toXYZ(self) -> "XYZ":
        return XYZ(
            self.x * self.Y / self.y, self.Y, (1 - self.x - self.y) * self.Y / self.y
        )


class XYZ(ColorSpace):
    X: float
    Y: float
    Z: float

    def _sum(self) -> float:
        return self.X + self.Y + self.Z

    def toxyY(self) -> "xyY":
        return xyY(self.X / self._sum(), self.Y / self._sum(), self.Y)


print(ColorSpace.plan("XYZ", "xyY"))
