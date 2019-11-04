
using System;

namespace GrIso
{
    class ShortHashList
    {
        static RandQuick RandQuick = RandQuick.Shared;
        const int max_hash_check = 128 * 1024;
        public const ushort None = ushort.MaxValue;

        ushort[] hash_list = new ushort[] { None, None, None, None };
        int list_size = 0;
        int hash_shift;
        uint hash_mul;
        uint hash_add;

        public ShortHashList Clone()
        {
            var clone = new ShortHashList();
            Clone(clone);
            return clone;
        }

        public void Clone(ShortHashList clone)
        {            
            clone.list_size = list_size;
            clone.hash_shift = hash_shift;
            clone.hash_mul = hash_mul;
            clone.hash_add = hash_add;
            clone.hash_list = new ushort[hash_list.Length];
            for (int i = 0; i < hash_list.Length; ++i)
                clone.hash_list[i] = hash_list[i];
        }

        public int Count()
        {
            return list_size;
        }

        public void Append(ushort item)
        {
            if (list_size == hash_list.Length)
            {
                var short_list = new ushort[2 * list_size];
                for (int i = 0; i < hash_list.Length; ++i)
                    short_list[i] = hash_list[i];
                hash_list = short_list;
            }
            hash_list[list_size] = item;
            ++list_size;
        }

        void PlugNexts()
        {
            for (int i = hash_list.Length - 2; i >= 0; --i)
                if (hash_list[i] == None)
                    hash_list[i] = hash_list[i + 1];
        }

        public ushort First()
        {
            return hash_list[0];
        }

        public ushort Next(ushort item)
        {
            int hash_index = (int)((item * hash_mul + hash_add) >> hash_shift);
            return hash_index < hash_list.Length-1 ? hash_list[hash_index+1] : None;
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
            Array.Sort(hash_list);
            for (int i = 1; i < hash_list.Length; ++i)
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

            int hash_size = 1;
            while (hash_size < list_size)
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

            for (int hash_check = 1; ; ++hash_check)
            {
                if (hash_check > max_hash_check)
                    return false;

                RandQuick.Next();
                hash_mul = RandQuick.Next();
                hash_add = RandQuick.Next();

                for (int i = 0; i < hash_list.Length; ++i)
                    hash_list[i] = None;
                for (int i = 0; ; ++i)
                {
                    if (i == list_size)
                    {
                        PlugNexts();
                        return true;
                    }
                    int hash_index = (int)((short_list[i] * hash_mul + hash_add) >> hash_shift);
                    if (hash_list[hash_index] != None)
                        break;
                    hash_list[hash_index] = short_list[i];
                }
            }
        }

        public bool Find(ushort item)
        {
            int hash_index = (int)((item * hash_mul + hash_add) >> hash_shift);
            return item == hash_list[hash_index];
        }
    }
}