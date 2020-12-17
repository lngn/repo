
#pragma once
#include "RandQuick.h"
#include "vector"
#include "algorithm"

namespace GrIso
{
    class ShortHashList
    {
        const int max_hash_check = 128;
		const ushort None = -1;

	public:
		ShortHashList() = default;
		ShortHashList(const ShortHashList&) = default;

		ShortHashList(ShortHashList && that)
			: data_list(that.data_list), hash_list(that.hash_list)
			, hash_size(that.data_size), data_size(that.data_size)
			, hash_shift(that.hash_shift), hash_mul(that.hash_mul), hash_add(that.hash_add)
		{
		}


		void Append(int item)
		{
			data_list.push_back((ushort)item);
			++data_size;
		}

		bool Hash()
		{
			data_size = data_list.size();
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

			hash_list.resize(hash_size);

			uint min_collision = -1;
			for (int hash_check = 0; hash_check < max_hash_check; ++hash_check)
			{
				RandQuickShared.Next();
				uint hash_mul = RandQuickShared.Next();
				uint hash_add = RandQuickShared.Next();

				uint collision = 0;
				for (uint i = 0; i < hash_size; ++i)
					hash_list[i] = 0;
				for (uint i = 0; i < data_size; ++i)
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
					this->hash_mul = hash_add;
					this->hash_add = hash_add;
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
				if (i1 < hash_list[hash_index1] || hash_index1 + 1 != hash_size && i1 >= hash_list[hash_index1 + 1])
				{
					for (uint i2 = hash_list[hash_index1]; ; ++i2)
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

		bool Find(int item) const
		{
			ushort item2 = (ushort)item;
			uint hash_index = (item2 * hash_mul + hash_add) >> hash_shift;
			uint index = hash_list[hash_index];
			++hash_index;
			uint finish = hash_index < hash_size ? hash_list[hash_index] : data_size;
			while (index < finish)
			{
				if (data_list[index] == item2)
					return true;
				++index;
			}
			return false;
		}

		uint Count() const 
		{
			return data_size; 
		} 
		
		uint operator[] (int index) const
		{
			return data_list[index]; 
		}

	protected:
		std::vector<ushort>  data_list;
		std::vector<ushort>  hash_list;
		uint hash_size, data_size;
		int hash_shift;
		uint hash_mul;
		uint hash_add;

	private:
        bool DenseHash()
        {
            if (data_size<2)
            {
                hash_mul = 0;
                hash_add = 0;
                hash_shift = 0;
				hash_list.push_back(0);
                hash_size = 0;
                if (data_size > 0)
                    ++hash_size;
                return true;
            }

            ushort max_num = 0;
            for (uint i = 0; i < data_size; ++i)
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
			hash_list.resize(hash_size);
            for (uint i = 0; i < hash_size; ++i)
                hash_list[i] = 0;
            for (uint i = 0; i < data_size; ++i)
                hash_list[data_list[i]] = 1;
            ushort data_index = 0;
            for (uint i = 0; i < hash_size; ++i)
            {
                ushort next_index = hash_list[i];
                hash_list[i] = data_index;
                data_index += next_index;
            }

            return true;
        }

        bool Duplicates()
        {
			std::sort(data_list.begin(), data_list.end());
            for (uint i = 1; i < data_size; ++i)
                if (data_list[i - 1] == data_list[i])
                    return true;
            return false;
        }
	};
}