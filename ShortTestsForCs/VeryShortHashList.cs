
using System;

namespace GrIso
{
    class VeryShortHashList
    {
        static RandQuick RandQuick = RandQuick.Shared;
        const int max_hash_check = 1024;
        public const ushort None = ushort.MaxValue;

        protected ushort[] data_list = new ushort[] { None, None, None, None };
        protected int data_size = 0;

        protected ushort[] hash_list = null;
        protected int hash_size = 0;

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

        public int Count()
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
            hash_list[data_size] = item;
            ++data_size;
        }

        

        bool DenseHash()
        {
            if (list_size<2)
            {
                hash_mul = 0;
                hash_add = 0;
                this.hash_shift = 0;
                return true;
            }

            ushort max_num = 0;
            for (int i = 0; i < list_size; ++i)
                if (hash_list[i] > max_num)
                    max_num = hash_list[i];

            int hash_size = 1, hash_shift = 32;
            while (hash_size < list_size)
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
            var short_list = hash_list;
            hash_list = new ushort[hash_size];
            for (int i = 0; i < hash_list.Length; ++i)
                hash_list[i] = None;

            for (int i = 0; i < list_size; ++i)
                hash_list[short_list[i]] = short_list[i];

            PlugNexts();
            return true;
            
        }

        private bool Duplicates()
        {
            Array.Sort(hash_list, 0, list_size);
            for (int i = 1; i < list_size; ++i)
                if (hash_list[i - 1] == hash_list[i])
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

            var short_list = hash_list;
            hash_list = new ushort[hash_size];

            uint min_collision = -1;
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
                    uint hash_index = (short_list[i] * hash_mul + hash_add) >> hash_shift;
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
                if (hash_list[hash_index] == 0)
                    ++hash_list[hash_index];
            }

            ushort data_index = 0;
            for (uint i = 0; i < hash_size; ++i)
            {
                ushort hash_count = hash_list[i];
                hash_list[i] = data_index;
                data_index += hash_count;
            }

            for (uint i = 0; i < data_size;)
            {
                uint hash_index = (data_list[i] * hash_mul + hash_add) >> hash_shift;
                if (hash_list[hash_index] < i || hash_index + 1 != hash_size && i >= hash_list[hash_index + 1])
                {
                    data_index = hash_list[hash_index];
                    while ()
                    {
                        uint hash_index = (data_list[data_index] * hash_mul + hash_add) >> hash_shift;
                    }
                }
                else ++i;
            }
        }

        public bool Find(ushort item)
        {
            int hash_index = (int)((item * hash_mul + hash_add) >> hash_shift);
            return item == hash_list[hash_index];
        }
    }
}