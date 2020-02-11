#include "indicators.h"
#include <iostream>
#include <phpcpp.h>
#include <stdio.h>
#include <stdlib.h>

/**
 *  Namespace to use
 */
using namespace std;

void print_array(const double *p, const int size) {
    int i;
    for (i = 0; i < size; ++i) {
        if (i) printf(", ");
        printf("%.1f", p[i]);
    }
    printf("\n");
}

/**
 *  tulip()
 */
void tulip()
{
//    const double data_in[] = {5,8,12,11,9,8,7,10,11,13};
//    const int input_length = sizeof(data_in) / sizeof(double);
//
//    printf("We have %d bars of input data.\n", input_length);
//    print_array(data_in, input_length);
//
//    const double options[] = {3};
//    printf("Our option array is: ");
//    print_array(options, sizeof(options) / sizeof(double));
//
//    const int start = ti_sma_start(options);
//    printf("The start amount is: %d\n", start);
//
//    const int output_length = input_length - start;
//    double *data_out = (double *) malloc((unsigned int)output_length * sizeof(double));
//    assert(data_out != 0);
//    printf("The output length is: %d\n", output_length);
//
//    const double *all_inputs[] = {data_in};
//    double *all_outputs[] = {data_out};
//    int error = ti_sma(input_length, all_inputs, options, all_outputs);
//    assert(error == TI_OKAY);
//
//
//    printf("The output data is: ");
//    print_array(data_out, output_length);


    printf("This program is an example of looping through\n");
    printf("each of the available indicators.\n\n");

    /* Set info to first indicators in array. */
    const ti_indicator_info *info = ti_indicators;

    /* The last item is all zeros, so we'll stop when we get there. */
    while (info->name != 0) {
        int i;

        printf("%s (%s) has type %d with: %d inputs, %d options, %d outputs.\n",
                info->name,
                info->full_name,
                info->type,
                info->inputs,
                info->options,
                info->outputs
              );


        printf("   inputs: ");
        for (i = 0; i < info->inputs; ++i)
            printf("%s%s", i ? ", " : "", info->input_names[i]);
        printf("\n");

        printf("   options: ");
        for (i = 0; i < info->options; ++i)
            printf("%s%s", i ? ", " : "", info->option_names[i]);
        printf("\n");

        printf("   outputs: ");
        for (i = 0; i < info->outputs; ++i)
            printf("%s%s", i ? ", " : "", info->output_names[i]);
        printf("\n");


        /* To use this indicator, call the start function like:
         * info->start(options);
         * and then call the actual indicator function like:
         * info->indicator(size, inputs, options, outputs);
         */
        printf("\n");

        ++info; /* Next indicator. */
    }
}


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

        // @todo    add your own functions, classes, namespaces to the extension
        // add function to extension
        extension.add<tulip>("tulip");


        // return the extension
        return extension.module();
    }
}