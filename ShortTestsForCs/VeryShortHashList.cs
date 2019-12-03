
using System;

namespace GrIso
{
    class VeryShortHashList
    {
        static RandQuick RandQuick = RandQuick.Shared;
        const int max_hash_check = 1024;
        public const ushort None = ushort.MaxValue;

        protected ushort[] data_list = new ushort[] { None, None, None, None };
        protected uint data_size = 0;

        protected ushort[] hash_list = null;
        protected uint hash_size = 0;

        protected int hash_shift;
        protected uint hash_mul;
        protected uint hash_add;

        public VeryShortHashList Clone()
        {
            var clone = new VeryShortHashList();
            Clone(clone);
            return clone;
        }

        public void Clone(VeryShortHashList clone)
        {            
            clone.data_size = data_size;
            clone.data_list = new ushort[data_list.Length];
            for (int i = 0; i < data_list.Length; ++i)
                clone.data_list[i] = data_list[i];
            clone.hash_shift = hash_shift;
            clone.hash_mul = hash_mul;
            clone.hash_add = hash_add;
            clone.hash_list = new ushort[hash_list.Length];
            for (int i = 0; i < hash_list.Length; ++i)
                clone.hash_list[i] = hash_list[i];
        }

        public uint Count()
        {
            return data_size;
        }

        public void Append(ushort item)
        {
            if (data_size == data_list.Length)
            {
                var short_list = new ushort[2 * data_size];
                for (int i = 0; i < data_list.Length; ++i)
                    short_list[i] = data_list[i];
                data_list = short_list;
            }
            data_list[data_size] = item;
            ++data_size;
        }

        

        bool DenseHash()
        {
            if (data_size<2)
            {
                hash_mul = 0;
                hash_add = 0;
                hash_shift = 0;
                hash_list = new ushort[1];
                hash_list[0] = 0;
                hash_size = 0;
                if (data_size > 0)
                    ++hash_size;
                return true;
            }

            ushort max_num = 0;
            for (int i = 0; i < data_size; ++i)
                if (data_list[i] > max_num)
                    max_num = data_list[i];

            hash_size = 1;
            hash_shift = 32;
            while (hash_size < data_size)
            {
                hash_size <<= 1;
                --hash_shift;
            }
            if (max_num >= hash_size)
            {
                hash_size <<= 1;
                --hash_shift;
            }

            if (max_num >= hash_size)
                return false;
             
            hash_mul = 1u <<hash_shift;
            hash_add = 0;
            this.hash_shift = hash_shift;
            hash_list = new ushort[hash_size];
            for (int i = 0; i < hash_list.Length; ++i)
                hash_list[i] = 0;
            for (int i = 0; i < data_size; ++i)
                hash_list[data_list[i]] = 1;
            ushort data_index = 0;
            for (int i = 0; i < hash_list.Length; ++i)
            {
                ushort next_index = hash_list[i];
                hash_list[i] = data_index;
                data_index += next_index;
            }


            return true;
            
        }

        private bool Duplicates()
        {
            Array.Sort(data_list, 0, (int)data_size);
            for (int i = 1; i < data_size; ++i)
                if (data_list[i - 1] == data_list[i])
                    return true;
            return false;
        }

        public bool Hash()
        {
            if (Duplicates())
                return false;
            if (DenseHash())
                return true;

            hash_size = 1;
            while (hash_size < data_size)
            {
                hash_size <<= 1;
            }
            hash_size <<= 1;

            hash_shift = 32;
            uint shift_check = 1;
            while (shift_check != 0 && shift_check != hash_size)
            {
                shift_check <<= 1;
                --hash_shift;
            }
            if (shift_check == 0)
                return false;

            hash_list = new ushort[hash_size];

            uint min_collision = uint.MaxValue; ;
            for (int hash_check = 0; hash_check < max_hash_check; ++hash_check)
            {
                RandQuick.Next();
                uint hash_mul = RandQuick.Next();
                uint hash_add = RandQuick.Next();

                uint collision = 0;
                for (int i = 0; i < hash_size; ++i)
                    hash_list[i] = 0;
                for (int i = 0; i < data_size; ++i)
                {
                    uint hash_index = (data_list[i] * hash_mul + hash_add) >> hash_shift;
                    if (hash_list[hash_index] == 0)
                        hash_list[hash_index] = 1;
                    else
                        ++collision;
                }
                if (collision < min_collision)
                {
                    min_collision = collision;
                    this.hash_mul = hash_add;
                    this.hash_add = hash_add;
                }
            }

            for (uint i = 0; i < hash_size; ++i)
                hash_list[i] = 0;
            for (uint i = 0; i < data_size; ++i)
            {
                uint hash_index = (data_list[i] * hash_mul + hash_add) >> hash_shift;
                ++hash_list[hash_index];
            }

            ushort data_index = 0;
            for (uint i = 0; i < hash_size; ++i)
            {
                ushort hash_count = hash_list[i];
                hash_list[i] = data_index;
                data_index += hash_count;
            }

            for (uint i1 = 0; i1 < data_size;)
            {
                uint hash_index1 = (data_list[i1] * hash_mul + hash_add) >> hash_shift;
                if (i1<hash_list[hash_index1]  || hash_index1 + 1 != hash_size && i1 >= hash_list[hash_index1 + 1])
                {
                    for ( uint i2= hash_list[hash_index1]; ;++i2)                    
                    {
                        uint hash_index2 = (data_list[i2] * hash_mul + hash_add) >> hash_shift;
                        if (hash_index1 != hash_index2)
                        {
                            ushort item = data_list[i1];
                            data_list[i1] = data_list[i2];
                            data_list[i2] = item;
                            break;
                        }
                    }
                }
                else ++i1;
            }

            return true;
        }

        public bool Find(ushort item)
        {
            uint hash_index = (item * hash_mul + hash_add) >> hash_shift;
            uint index = hash_list[hash_index];
            ++hash_index;
            uint finish = hash_index < hash_size ? hash_list[hash_index] : data_size;
            while (index<finish)
            {
                if (data_list[index] == item)
                    return true;
                ++index;
            }
            return false;
        }
    }
}