#ifndef NVPARSE_READ_HPP_INCLUDED
#define NVPARSE_READ_HPP_INCLUDED

#include <vector>
#include <string>
#include <fstream>
#include <stdexcept>

#include "node.hpp"

namespace nvparsehtml
{
    //! Represents data loaded from a file
    template<class Ch>
    class File
    {
        
    public:
        
        //! Loads file into the memory. Data will be automatically destroyed by the destructor.
        //! \param filename Filename to load.
        File(const char *filename)
        {
            using namespace std;

            // Open stream
            basic_ifstream<Ch> stream(filename, ios::binary);
            if (!stream)
                throw runtime_error(string("cannot open file ") + filename);
            stream.unsetf(ios::skipws);
            
            // Determine stream size
            stream.seekg(0, ios::end);
            size_t size = stream.tellg();
            stream.seekg(0);   
            
            // Load data and add terminating 0
            m_data.resize(size / sizeof(Ch) + 1);
            stream.read(&m_data.front(), static_cast<streamsize>(size));
            m_data[size] = 0;
        }

        //! Loads file into the memory. Data will be automatically destroyed by the destructor
        //! \param stream Stream to load from
        File(std::basic_istream<Ch> &stream)
        {
            using namespace std;

            // Load data and add terminating 0
            stream.unsetf(ios::skipws);
            m_data.assign(istreambuf_iterator<Ch>(stream), istreambuf_iterator<Ch>());
            if (stream.fail() || stream.bad())
                throw runtime_error("error reading stream");
            m_data.push_back(0);
        }
        
        //! Gets file data.
        //! \return Pointer to data of file.
        Ch *data()
        {
            return &m_data.front();
        }

        //! Gets file data.
        //! \return Pointer to data of file.
        const Ch *data() const
        {
            return &m_data.front();
        }

        //! Gets file data size.
        //! \return Size of file data, in characters.
        std::size_t size() const
        {
            return m_data.size();
        }

    private:

        std::vector<Ch> m_data;   // File data

    };
}
#endif
