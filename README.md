ltvc
=======
This is a C implementation of the [LightVectors](litevector.org) serialization format. 

The serialization and deserialization is contained in a single header (litevectors.h) and source file (litevectors.c). 

The decoder is implemented as a streaming parser - no memory allocation is performed in the base library.

