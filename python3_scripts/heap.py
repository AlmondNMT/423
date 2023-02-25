class Heap:
    def __init__(self, A):
        self.heapsize = len(A)
        self.A = A
        # Build heap algo
        for i in range(self.heapsize // 2 + 2, 0, -1):
            self._heapify(i)
    def _heapify(self, i):
        l = self._left(i)
        r = self._right(i)
        if l <= self.heapsize and self.A[l-1] > self.A[i-1]:
            largest = l
        else:
            largest = i
        if r <= self.heapsize and self.A[r-1] > self.A[largest-1]:
            largest = r
        if largest != i:
            self.A[i-1], self.A[largest-1] = self.A[largest-1], self.A[i-1]
            self._heapify(largest)

    def _left(self, i):
        return 2 * i

    def _right(self, i):
        return 2 * i + 1

    def _parent(self, i):
        return i // 2

    def change(self, i, delta):
        self.A[i-1] += delta
        p = self._parent(i)
        l = self._left(i)
        r = self._right(i)
        if l <= self.heapsize and self.A[i-1] < self.A[l-1]:
            self._heapify(i)
        if r <= self.heapsize and self.A[i-1] < self.A[r-1]:
            self._heapify(i)
        while p >= 1 and self.A[p-1] < self.A[i-1]:
            self.A[p-1], self.A[i-1] = self.A[i-1], self.A[p-1]
            p = self._parent(p)
            i = self._parent(i)

    def _parent(self, i):
        return i // 2

    def __len__(self):
        return len(self.A)

    def __getitem__(self, i):
        return self.A[i]

    def __setitem__(self, i, val):
        A[i] = val

    def __repr__(self):
        return self.__str__()

    def __str__(self):
        return "{}".format(self.A)


def heapify(A, i):
    """
    unrelated to the class above
    """
    l = 2 * i
    r = 2 * i + 1
    if l < len(A) and A[l] > A[i]:
        largest = l
    else:
        largest = i
    if r < len(A) and A[r] > A[largest]:
        largest = r
    if largest != i:
        A[i], A[largest] = A[largest], A[i]
        heapify(A, largest)

def buildheap(A):
    n = len(A)
    for i in range(n // 2, -1, -1):
        heapify(A, i)

def heapsort(heap):
    n = len(heap)
    for i in range(n - 1, 0, -1):
        heap[0], heap[i] = heap[i], heap[0]
        

A = [1, 2, 3, 4, 5]
buildheap(A)
print(A)
