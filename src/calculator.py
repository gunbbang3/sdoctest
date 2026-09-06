# @relation(ARCH-1, scope=file)


def add(a, b):
    if not isinstance(a, (int, float)) or not isinstance(b, (int, float)):
        raise TypeError("add() requires numeric arguments")
    return a + b
