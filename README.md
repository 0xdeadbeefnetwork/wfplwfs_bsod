# wfplwfs_bsod
A local DoS PoC for Windows 8.1 to 10/11. Rated medium by MSRC and closed without fix.
API hashing was used because I forgot to shut off automatic sample submission on the original PoC and burned it.
Windows defender flagged it.
May be used in tandem with adapter_guid.c to find the interface GUID to make this work for you.
OID_PNP_QUERY_POWER is the OID in question.
