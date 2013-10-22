JNP2
====

Tutaj sugeruję umieszczać wszelakie uwagi i wątpliwości na bieżąco, gdybyśmy nie pamiętami o tym później.

1. Kod debugujący.
Peczarski zamieścił linki do tych czytanek. W pierwszej z nich jest wyraźnie powiedziane, aby realizować
to w taki sposób.

#ifndef NDEBUG
    const bool debug = true;
#else
    const bool debug = false;

#endifi

A w samym kodzie.

void buggy() {
    if( debug ) {
        // some debugging code...
    }
    // some actual code...
    if( debug ) {
        // more debugging code...
    }
}

Bo ponoć kompilator jakoś to widzi. 
