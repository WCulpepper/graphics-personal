1) The size of a record is 3.
2) The offset of the address of the second record (AoS) is the size of a single record. The length is also a single record.
3) The offset of the address of the second record (SoA) is the size of an int + the size of a float + the size of a char. The length is also the sum of these values.
4) AoS is more advantageous if you need bulk data, e.g. a bunch of points for a particle system.
5) SoA is more advantageous if you need more precise control over your data.
