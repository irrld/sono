import io

module io {
    extern fun println(s: String);
}


// This is a comment
fun main() {
    String hello = "Hello World!"; // This is also a comment
    Int32 num = 5;
    Int64 num = 5L;
    Float num = 5.0f;
    Double num = 5.0;
    Char num = 'a';
    io.println(hello);
    /*
    This is a multiline comment
    // This is a nested comment
    */
}
