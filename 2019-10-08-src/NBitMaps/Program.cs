using System;
using System.IO;
using System.IO.MemoryMappedFiles;
using System.Runtime.InteropServices;

namespace NBitMaps {

    class Program {

        const String ImageFileName = @"..\..\Jellyfish1.bmp";

        static int SizeofBitMapFileHeader = Marshal.SizeOf(typeof(BitMapFileHeader));
        static int SizeofRgbTriple = Marshal.SizeOf(typeof(RgbTriple));
        static int SizeofBitMapInfoHeader = Marshal.SizeOf(typeof(BitMapInfoHeader));

        static void Main(string[] args) {

            Console.WriteLine("sizeof(BitMapFileHeader)={0}", SizeofBitMapFileHeader);
            Console.WriteLine("sizeof(RgbTriple)={0}", SizeofRgbTriple);
            Console.WriteLine("sizeof(BitMapInfoHeader)={0}", SizeofBitMapInfoHeader);

            // Create the memory-mapped file.
            using (var mmf = MemoryMappedFile.CreateFromFile(ImageFileName, 
                FileMode.Open)) {
                using (var accessor = mmf.CreateViewAccessor(0, 0)) {
                    
                    RgbTriple color;
                    BitMapFileHeader header;
                    BitMapInfoHeader info;

                    accessor.Read(0, out header);
                    accessor.Read(SizeofBitMapFileHeader, out info);
                    info.Show();

                    header.Show();

                    // Make changes to the view.
                    long    start = header.offBits, 
                            end = header.offBits + info.height * info.width * SizeofRgbTriple;
                   
                    for (long i = start; i <= end; i += SizeofRgbTriple) {
                        accessor.Read(i, out color);
                        color.Brighten(80);
                        accessor.Write(i, ref color);
                    }
                    
                   
                }
            }
        }
    }

    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public struct BitMapFileHeader {
        public ushort type;
        public uint size;
        public ushort reserved1;
        public ushort reserved2;
        public uint offBits;

        public void Show() {
            Console.WriteLine("type={0}", type);
            Console.WriteLine("size={0}", size);
            Console.WriteLine("offBits={0}", offBits);
            
        }
    }

    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public struct RgbTriple {
        public byte blue;
        public byte green;
        public byte red;
       
        // Make the view brighter.
        public void Brighten(byte value) {
            red =  red > byte.MaxValue-value ? byte.MaxValue :  (byte) (red + value);
            green = green > byte.MaxValue - value ? byte.MaxValue : (byte)(green + value);
            blue = blue > byte.MaxValue - value ? byte.MaxValue : (byte)(blue + value);
        }
    }

    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public struct BitMapInfoHeader {
        public uint size;
        public int width;
        public int height;
        public ushort planes;
        public ushort bitCount;
        public uint compression;
        public uint sizeImage;
        public int xPelsPerMeter;
        public int yPelsPerMeter;
        public uint clrUsed;
        public uint clrImportant;

        public void Show() {
            Console.WriteLine("size={0}", size);
            Console.WriteLine("width={0}", width);
            Console.WriteLine("height={0}", height);
            Console.WriteLine("sizeImage={0}", sizeImage);
            Console.WriteLine("bitCount={0}", bitCount);
        }
    }
    
}
