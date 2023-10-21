#pragma once

#ifndef __SORT

#define __SORT

/*
排序问题(sorting problem)
When we want to sort numbers, it's often because they are the keys associated with other data, 
which we call satellite data. Together, a key and satellite data form a record. For example,
consider a spreadsheet containing student records with many associated pieces of data such as age,
grade-point average, and number of courses taken. Any one of these quantities could be a key,
but when the spreadsheet sorts, it moves the associated record (the satellite data) with the key.
When describing a sorting algorithm, we focus on the keys, but it is important to remember that there
usually is associated satellite data.
当我们想对数组进行排序时，通常是因为它们是与其他数据（卫星数据）相关联的键。键和卫星数据一起形成记录。
例如，考虑一个包含学生记录的电子表格，其中包含许多关联数据，如年龄、平均绩点(GPA)和所修课程数。
这些量中的任何一个都可能是键，但当电子表格排序时，它会用键移动相关记录（卫星数据）。
在描述排序算法时，我们将重点放在键上，但重要的是要记住，通常存在相关的卫星数据。
*/

namespace kit
{
    /*
    INSERTION-SORT(A, n)
    for i = 2 to n
        key = A[i]
        j = i - 1
        while j > 0 and A[j] > key
            A[j + 1] = A[j]
            j = j - 1
        A[j + 1] = key
    */
    //插入排序
    void insertion_sort(int* _array, size_t size)
    {
        for (size_t i = 1; i < size; i++)
        {
            int current = _array[i];
            size_t j = i - 1;
            while ( j >= 0 && _array[j] > current)
            {
                _array[j + 1] = _array[j];
                j--;
            }
            _array[j + 1] = current;
        }
    }
}

#endif // !__SORT
