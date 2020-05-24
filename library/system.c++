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
