#include <assert.h>    
#include <stdio.h>     
#include <math.h>      
#include <complex.h>   

float pi; 

/* Function to compute the Discrete Fourier Transform (DFT)
* in[]: Input array of real numbers
* out[]: Output array of complex numbers
* n: Number of elements in the input array
*/
void dft(float in[], float complex out[], size_t n)
{
    for (size_t f = 0; f < n; ++f) {
        out[f] = 0;
        for (size_t i = 0; i < n; ++i) {
            // Calculate the normalized frequency index
            float t = (float)i / n;
            // Apply the DFT formula to compute the frequency component
            out[f] += in[i] * cexp(2 * I * pi * f * t);
        }
    }
}

/*  Function to compute the Fast Fourier Transform (FFT) using the Cooley-Tukey algorithm
*   in[]: Input array of real numbers
*   stride: Step size to access elements in the input array
*   out[]: Output array of complex numbers
*   n: Number of elements in the input array
*/
void fft(float in[], size_t stride, float complex out[], size_t n)
{
    assert(n > 0);  // Ensure that the number of elements is positive

    if (n == 1) {
        // Base case: If only one element, the FFT is the same as the input
        out[0] = in[0];
        return;
    }

    // Recursively compute the FFT for even and odd indexed elements
    fft(in, stride * 2, out, n / 2);
    fft(in + stride, stride * 2, out + n / 2, n / 2);

    for (size_t k = 0; k < n / 2; ++k) {
        // Calculate the angle for the current frequency component
        float t = (float)k / n;
        // Compute the complex exponential term
        float complex v = cexp(-2 * I * pi * t) * out[k + n / 2];
        float complex e = out[k];
        // Combine the results to get the FFT output
        out[k]       = e + v;
        out[k + n / 2] = e - v;
    }
}

int main()
{
    pi = atan2f(1, 1) * 4;  // Compute the value of pi

    size_t n = 8;  // Number of samples, must be a power of 2 for the FFT
    float in[n];   // Input array of real numbers
    float complex out[n];  // Output array of complex numbers
    
    // Initialize the input array with a sample signal
    for (size_t i = 0; i < n; ++i) {
        float t = (float)i / n;
        in[i] = cosf(2 * pi * t * 1) + sinf(2 * pi * t * 2) + cosf(2 * pi * t * 3);
    }

    fft(in, 1, out, n);  // Compute the FFT of the input signal
    // dft(in, out, n);   // Uncomment to use the DFT function instead

    // Print the results of the FFT
    for (size_t f = 0; f < n; ++f) {
        printf("%02zu: %.2f, %.2f\n", f, creal(out[f]), cimag(out[f]));
    }

    return 0;
}
