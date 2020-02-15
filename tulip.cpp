#include "indicators.h"
#include <phpcpp.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <iostream>

/**
 *  Namespace to use
 */
using namespace std;

/**
* Tulipe indicators class 
*/
class Tulind : public Php::Base
{  
private:
    std::map<std::string, Php::Value> _indicators;  

public:
    /**
     *  C++ constructor and destructor
     */
    Tulind() = default;
    virtual ~Tulind() = default;

    /**
     *  php "constructor"
     */
    void __construct()
    {
        const ti_indicator_info *info = ti_indicators;

        while (info->name != 0) {   
            Php::Value indicator;

            indicator["name"] = info->name;
            indicator["full_name"] = info->full_name;

            char const *type = "unknown";

            switch (info->type) {
                case TI_TYPE_OVERLAY:
                    type = "overlay";
                    break;
                case TI_TYPE_INDICATOR:
                    type = "indicator";
                    break;
                case TI_TYPE_MATH:
                    type = "math";
                    break;
                case TI_TYPE_SIMPLE:
                    type = "simple";
                    break;
                case TI_TYPE_COMPARATIVE:
                    type = "comparative";
                    break;
            }

            indicator["type"] = type;

            Php::Array inputs;
            Php::Array options;
            Php::Array outputs;

            int i;
            for (i = 0; i < info->inputs; ++i)
                inputs[i] = info->input_names[i];         

            for (i = 0; i < info->options; ++i)
                options[i] = info->option_names[i];       

            for (i = 0; i < info->outputs; ++i)
                outputs[i] = info->output_names[i];


            indicator["inputs"] = inputs;
            indicator["options"] = options;
            indicator["outputs"] = outputs;

             _indicators[info->name] = indicator;
            
            ++info;
        }
    }

    /**
     *  Return the version
     *  @return     string
     */
    Php::Value version() { return ti_version(); }

    /**
     *  Get indicators list
     *  @return     array
     */
    Php::Value indicators() { return _indicators; }

    Php::Value __call(const char *name, Php::Parameters &parameters)
    {
        const ti_indicator_info *info = ti_find_indicator(name);

        if (info == 0) {
            throw Php::Exception("Method not found");
        }

        if (!Php::is_array(parameters[0]) ) {
            throw Php::Exception("Expecting first argument to be array of inputs.");
        }

        if (!Php::is_array(parameters[1]) ) {
            throw Php::Exception("Expecting first argument to be array of options.");
        }

        double data_in[parameters[0].size()];
        for (int i = 0; i < parameters[0].size(); i++) {
            data_in[i] = parameters[0][i];
        }

        const int input_length = sizeof(data_in) / sizeof(double);

        double options[parameters[1].size()];
        for (int i = 0; i < parameters[1].size(); i++) {
            options[i] = parameters[1][i];
        }

        const int start = info->start(options);
        const int output_length = input_length - start;
        double *data_out = (double *) malloc((unsigned int)output_length * sizeof(double));
        assert(data_out != 0);

        const double *all_inputs[] = {data_in};
        double *all_outputs[] = {data_out};
        int error = info->indicator(input_length, all_inputs, options, all_outputs);
        assert(error == TI_OKAY);

        Php::Array out;
        for(int i = 0; i < output_length; i++) {
            out[i] = data_out[i];
        }
        
        return out;        
    }
};


/**
 *  tell the compiler that the get_module is a pure C function
 */
extern "C" {
    /**
     *  Function that is called by PHP right after the PHP process
     *  has started, and that returns an address of an internal PHP
     *  strucure with all the details and features of your extension
     *
     *  @return void*   a pointer to an address that is understood by PHP
     */
    PHPCPP_EXPORT void *get_module() 
    {
        // static(!) Php::Extension object that should stay in memory
        // for the entire duration of the process (that's why it's static)
        static Php::Extension extension("tulip", "1.0");

         // description of the class so that PHP knows which methods are accessible
        Php::Class<Tulind> tulind("Tulind");
        tulind.method<&Tulind::__construct> ("__construct");
        tulind.method<&Tulind::version> ("version");
        tulind.method<&Tulind::indicators> ("indicators");

        // add the class to the extension
        extension.add(std::move(tulind));

        // return the extension
        return extension;
    }
}