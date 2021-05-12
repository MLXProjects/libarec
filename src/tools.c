#include <arec_internal.h>

char *arec_version(int type){
	char *version=malloc(16);
	switch (type){
	case AREC_REQVERSION_FULL:
		snprintf(version, 16,"libarec %i.%i.%i",
				AREC_VERSION_MAJOR,
				AREC_VERSION_MINOR,
				AREC_VERSION_PATCH);
		break;
	case AREC_REQVERSION_MAJOR:
		snprintf(version, 16,"%i",
				AREC_VERSION_MAJOR);
		break;
	case AREC_REQVERSION_MINOR:
		snprintf(version, 16,"%i",
				AREC_VERSION_MINOR);
		break;
	case AREC_REQVERSION_PATCH:
		snprintf(version, 16,"%i",
				AREC_VERSION_PATCH);
		break;
	case AREC_REQVERSION_NUMBER:
	default:
		snprintf(version, 16,"%i.%i.%i",
				AREC_VERSION_MAJOR,
				AREC_VERSION_MINOR,
				AREC_VERSION_PATCH);
		break;
	}
	return version;
}