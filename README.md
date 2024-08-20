a variable rate of 500 Hz to 10 kHz depending on configuration.

> | 15   | 14   | 13   | 12   | 11   | 10   | 9    | 8    | 7    | 6    | 5    | 4    | 3    | 2   | 1    | 0    |
> |------|------|------|------|------|------|------|------|------|------|------|------|------|-----|------|------|
> | HRZ3 | HRZ2 | HRZ1 | HRZ0 | SIGN | VAL7 | VAL6 | VAL5 | VAL4 | VAL3 | VAL2 | VAL1 | VAL0 | CLR | CNF1 | CNF0 |
>
>  HRZ(3:0) is a time horizon of the estimate, where the time is expressed with the exponential
> 
>     t_valid = 16.5*2^HRZ  (ms)
> 
> From the time of this sample (inclusive) through the time horizon (exclusive), the host should
> take this sample into account. When the time horizon expires, the host should discard this
> sample.
> 
> SIGN is 1 if this sample's value is greater than the estimated true value, or 0 if this
> sample's value is lower than the estimated true value.
> 
> VAL(7:0) is a signed two's-complement value representing the offset in units of 390 lx from the
> reference illuminance. The reference illuminance is factory-calibrated to 50,000 lx.
> 
> If CLR is 1, the host should discard the effects of all prior samples (the content of this sample
> is still valid).
> 
> CNF(1:0) is a confidence metric from 0-3. If the photometer issues two conflicting samples, the
> sample with the higher confidence should override the sample with lower confidence. If two
> samples having the same confidence conflict, the newer sample should override the older sample.
