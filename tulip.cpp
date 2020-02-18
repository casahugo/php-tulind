#include "indicators.h"
#include <phpcpp.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include <math.h>

void print_array(const double *p, const int size) {
    int i;
    for (i = 0; i < size; ++i) {
        if (i) printf(", ");
        printf("%.1f", p[i]);
    }
    printf("\n");
}

static double *getArray(Php::Value parameter, const int size, int offset) {
    Php::Value inputs = parameter[offset];
    double *input = (double*)malloc(sizeof(double) * size);

    for (int i = 0; i < inputs.size(); i++) {
        double value = inputs[i];
        input[i] = value;
    }
    return input;
}



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

        int size = 0;
        {
            for (int i = 0; i < parameters[0].size(); i++) {
                Php::Value inputs = parameters[0][i];
                if (inputs.size() < size || size == 0) {
                    size = inputs.size();
                }
            }
        }

        TI_REAL out[5][size];
        TI_REAL const *inputs[] = {0, 0, 0, 0, 0};
        TI_REAL *outputs[] = {out[0], out[1], out[2], out[3], out[4]};

        int r=0;

        /* Setup the inputs. */
        int j;
        for (j = 0; j < info->inputs; ++j) {
            if (strcmp(info->input_names[j], "open") == 0) {
                inputs[j] = getArray(parameters[0], size, j);
            } else if (strcmp(info->input_names[j], "high") == 0) {
                inputs[j] = getArray(parameters[0], size, j);
            } else if (strcmp(info->input_names[j], "low") == 0) {
                inputs[j] = getArray(parameters[0], size, j);
            } else if (strcmp(info->input_names[j], "close") == 0) {
                inputs[j] = getArray(parameters[0], size, j);
            } else if (strcmp(info->input_names[j], "volume") == 0) {
                inputs[j] = getArray(parameters[0], size, j);
            } else if (strcmp(info->input_names[j], "real") == 0) {
                if (!r) {
                    inputs[j] = getArray(parameters[0], size, j); r = 1;
                } else {
                    inputs[j] = getArray(parameters[0], size, j);
                }
            } else {
                return 0;
            };
        }

        /* See if we need alternative inputs (for the indicators that can't take large numbers. */
        const char *alts[] = {"acos", "asin", "atan", "cosh", "sinh", "tanh", "todeg", 0};
        {
            const char **alt = alts;
            while (*alt) {
                if (strcmp(*alt, info->name) == 0) {
                    r=0;
                    for (j = 0; j < info->inputs; ++j)
                        inputs[j] = getArray(parameters[0], size, j);

                    break;
                }
                ++alt;
            }
        }

        /* Set options, save offset. */
        TI_REAL options[10];
        {
            int i;
            for (i = 0; i < info->options; ++i) {
                options[i] = parameters[1][i];
            }
        }

        int start = info->start(options);

        /* Run it. */
        const int ret = info->indicator(size, inputs, options, outputs);

        Php::Value output;
        if (ret == TI_OKAY) {
            for (int k = 0; k < info->outputs; ++k) {
                Php::Value value;
                for (int i = 0; i < size; ++i) {
                    if (i >= start) {
                        value[i] = out[k][i - start];
                    }
                }

                output[k] = value;
            }
        }
        return output;
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