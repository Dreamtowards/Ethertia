
## stdout, stdin.

为什么输出不是 print 字眼，而是 write?

```

void main() {

    System::out.writeln("Sth");
    
    System::in.readline();

    Console::writeln("Sth");
    
    String s = Console::readln();
}

```

## Iterators




std::itr::range

```
// code segments.

for (i32 i : std::itr::range(100)) {
    
}

for (f32 f : std::itr::range(0, 100, 0.1)) {

}

// stl impl

class Iterator<T> {

    T current();
    
    bool next();

}

RangeIterator<T> range<T>(T from, T to, T step) {
    return Iterator<T>() {
        T i = from;
        T current() {
            return i;
        }
        bool next() {
            i += step;
            return i < to;
        }
    };
}


```