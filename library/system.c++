struct type__system
{
    struct type__console
    {
        template<class T>
        static void operator_lt_lt_lt (T text)
        {
            std::cout << text << std::endl;
        }
    };
    type__console console;
};
type__system system;



void type__system::type__console::operator_lt_lt_lt (ae::span data)
{
    std::cout::write((char*)data.data, data.size);
    std::cout::flush();
}
